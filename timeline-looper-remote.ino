#include <MIDI.h>
#include <EasyButton.h>

MIDI_CREATE_DEFAULT_INSTANCE();

#define channel 1
#define value 127
#define ccStop 85
#define ccPlay 86
#define ccRecord 87
#define ccUndo 89
#define ccReverse 94
#define ccHalfSpeed 95
#define ccLevel 98
#define initLoopLevel 127
#define inputDelay 35
#define ledPage 9
#define ledRecord 7
#define ledPlay 8

const unsigned short LONG_PRESS = 1000;
const unsigned short LEVEL_DELAY = 80; // 10s to reduce looper level from 100% to 0% (10000 / 127)

EasyButton btnPage(4, inputDelay, false, false);
EasyButton btnStop(5, inputDelay, false, false);
EasyButton btnRecord(2, inputDelay, false, false);
EasyButton btnPlay(3, inputDelay, false, false);

bool recording = false;
bool playing = false;
bool reverseActive = false;
bool halfSpeedActive = false;
bool pageLongPress = false;
bool recordLongPress = false;
unsigned short page = 1;
unsigned short loopLevel = initLoopLevel;
unsigned long levelTimer = 0;

void switchPage() {
  if (page == 1) {
    page = 2;

    // Update LEDs
    digitalWrite(ledPage, true);
    digitalWrite(ledRecord, reverseActive);
    digitalWrite(ledPlay, halfSpeedActive);

  } else {
    page = 1;

    // Update LEDs
    digitalWrite(ledPage, false);
    digitalWrite(ledRecord, recording);
    digitalWrite(ledPlay, playing);
  }
}

void onPageLongPress() {
  pageLongPress = true;
}

void onRecordLongPress() {
  recordLongPress = true;
}

void setup() {
  Serial.begin(31250);
  pinMode(ledPage, OUTPUT);
  pinMode(ledRecord, OUTPUT);
  pinMode(ledPlay, OUTPUT);
  btnPage.begin();
  btnStop.begin();
  btnRecord.begin();
  btnPlay.begin();
  btnPage.onPressedFor(LONG_PRESS, onPageLongPress);
  btnRecord.onPressedFor(LONG_PRESS, onRecordLongPress);
}

void loop() {
  // Read buttons
  btnPage.read();
  btnRecord.read();
  btnPlay.read();
  btnStop.read();

  // Page selection and looper level
  if (btnPage.wasPressed()) {
    levelTimer = millis(); // Take time for looper level
    switchPage();
  } else if (btnPage.wasReleased()) {
    pageLongPress = false;
  } else if (pageLongPress && loopLevel > 0) {
    if (millis() - levelTimer > LEVEL_DELAY) {
      levelTimer = millis(); // Reset timer
      loopLevel--;
      MIDI.sendControlChange(ccLevel, loopLevel, channel);
    }

    // Switch back to page 1
    if (page == 2) {
      switchPage();
    }
  }

  // Page 1
  if (page == 1) {
    // Record
    if (btnRecord.wasPressed() || (recordLongPress && btnRecord.wasReleased())) {
      MIDI.sendControlChange(ccRecord, value, channel);

      if (!recording) {
        recording = true;
      } else if (recording && !playing) {
        playing = true;
        digitalWrite(ledPlay, playing);
      } else {
        recording = false;
      }

      digitalWrite(ledRecord, recording);
      recordLongPress = false;
    }

    // Play
    if (btnPlay.wasPressed()) {
      MIDI.sendControlChange(ccPlay, value, channel);

      if (!playing) {
        playing = true;
        digitalWrite(ledPlay, playing);
      }

      if (recording) {
        recording = false;
        digitalWrite(ledRecord, recording);
      }
    }

    // Stop
    if (btnStop.wasPressed()) {
      MIDI.sendControlChange(ccStop, value, channel);
      recording = false;
      playing = false;
      digitalWrite(ledRecord, false);
      digitalWrite(ledPlay, false);

      // Reset loop level
      if (loopLevel != initLoopLevel) {
        loopLevel = initLoopLevel;
        MIDI.sendControlChange(ccLevel, loopLevel, channel);
      }

       // Reset reverse
      if (reverseActive) {
        reverseActive = false;
        MIDI.sendControlChange(ccReverse, value, channel);
      }

      // Reset half speed
      if (halfSpeedActive) {
        halfSpeedActive = false;
        MIDI.sendControlChange(ccHalfSpeed, value, channel);
      }
    }
  }

  // Page 2
  else {
    // Reverse
    if (btnRecord.wasPressed()) {
      reverseActive = !reverseActive;
      MIDI.sendControlChange(ccReverse, value, channel);
      digitalWrite(ledRecord, reverseActive);
    }

    // Undo
    if (btnStop.wasPressed()) {
      MIDI.sendControlChange(ccUndo, value, channel);
    }

    // Half speed
    if (btnPlay.wasPressed()) {
      halfSpeedActive = !halfSpeedActive;
      MIDI.sendControlChange(ccHalfSpeed, value, channel);
      digitalWrite(ledPlay, halfSpeedActive);
    }
  }
}
