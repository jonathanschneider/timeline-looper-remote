#include <Arduino.h>
#include <MIDI.h>
#include <EasyButton.h>
#include "config.h"

MIDI_CREATE_DEFAULT_INSTANCE();

EasyButton btnRecord(PIN_BTN_RECORD, INPUT_DELAY, PULLUP, INVERT);
EasyButton btnPlay(PIN_BTN_PLAY, INPUT_DELAY, PULLUP, INVERT);
EasyButton btnPage(PIN_BTN_PAGE, INPUT_DELAY, PULLUP, INVERT);
EasyButton btnStop(PIN_BTN_STOP, INPUT_DELAY, PULLUP, INVERT);

bool recording = false;
bool playing = false;
bool reverseActive = false;
bool halfSpeedActive = false;
bool pageLongPress = false;
bool recordLongPress = false;
unsigned short page = 1;
unsigned short loopLevel = INIT_LOOP_LEVEL;
unsigned long levelTimer = 0;

void switchPage() {
  if (page == 1) {
    page = 2;

    // Update LEDs
    digitalWrite(LED_PAGE, true);
    digitalWrite(LED_RECORD, reverseActive);
    digitalWrite(LED_PLAY, halfSpeedActive);

  } else {
    page = 1;

    // Update LEDs
    digitalWrite(LED_PAGE, false);
    digitalWrite(LED_RECORD, recording);
    digitalWrite(LED_PLAY, playing);
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
  pinMode(LED_PAGE, OUTPUT);
  pinMode(LED_RECORD, OUTPUT);
  pinMode(LED_PLAY, OUTPUT);
  btnRecord.begin();
  btnPlay.begin();
  btnPage.begin();
  btnStop.begin();
  btnRecord.onPressedFor(LONG_PRESS, onRecordLongPress);
  btnPage.onPressedFor(LONG_PRESS, onPageLongPress);
}

void loop() {
  // Read buttons
  btnRecord.read();
  btnPlay.read();
  btnPage.read();
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
      MIDI.sendControlChange(CC_LEVEL, loopLevel, MIDI_CHANNEL);
    }

    // Switch back to page 1
    if (page == 2) {
      switchPage();
    }
  }

  // Page 1
  if (page == 1) {
    // Record
    if (btnRecord.wasPressed()) {
      MIDI.sendControlChange(CC_RECORD, MIDI_VALUE, MIDI_CHANNEL);

      if (!recording) {
        recording = true;
      } else if (recording && !playing) {
        playing = true;
        digitalWrite(LED_PLAY, playing);
      } else {
        recording = false;
      }

      digitalWrite(LED_RECORD, recording);
    } else if (recording && recordLongPress && btnRecord.wasReleased()) {

      if (!playing) { // Only recording
        MIDI.sendControlChange(CC_PLAY, MIDI_VALUE, MIDI_CHANNEL); // Send play to stop recording and start playing
        playing = true;
        digitalWrite(LED_PLAY, playing);
      } else { // Recording and playing
        MIDI.sendControlChange(CC_RECORD, MIDI_VALUE, MIDI_CHANNEL); // Send record to stop recording and not retrigger loop
      }

      recording = false;
      recordLongPress = false; // Reset long press
      digitalWrite(LED_RECORD, recording);
    }

    // Play
    if (btnPlay.wasPressed()) {
      MIDI.sendControlChange(CC_PLAY, MIDI_VALUE, MIDI_CHANNEL);

      if (!playing) {
        playing = true;
        digitalWrite(LED_PLAY, playing);
      }

      if (recording) {
        recording = false;
        digitalWrite(LED_RECORD, recording);
      }
    }

    // Stop
    if (btnStop.wasPressed()) {
      MIDI.sendControlChange(CC_STOP, MIDI_VALUE, MIDI_CHANNEL);
      recording = false;
      playing = false;
      digitalWrite(LED_RECORD, false);
      digitalWrite(LED_PLAY, false);

      // Reset loop level
      if (loopLevel != INIT_LOOP_LEVEL) {
        loopLevel = INIT_LOOP_LEVEL;
        MIDI.sendControlChange(CC_LEVEL, loopLevel, MIDI_CHANNEL);
      }

       // Reset reverse
      if (reverseActive) {
        reverseActive = false;
        MIDI.sendControlChange(CC_REVERSE, MIDI_VALUE, MIDI_CHANNEL);
      }

      // Reset half speed
      if (halfSpeedActive) {
        halfSpeedActive = false;
        MIDI.sendControlChange(CC_HALF_SPEED, MIDI_VALUE, MIDI_CHANNEL);
      }
    }
  }

  // Page 2
  else {
    // Reverse
    if (btnRecord.wasPressed()) {
      reverseActive = !reverseActive;
      MIDI.sendControlChange(CC_REVERSE, MIDI_VALUE, MIDI_CHANNEL);
      digitalWrite(LED_RECORD, reverseActive);
    }

    // Undo
    if (btnStop.wasPressed()) {
      MIDI.sendControlChange(CC_UNDO, MIDI_VALUE, MIDI_CHANNEL);
    }

    // Half speed
    if (btnPlay.wasPressed()) {
      halfSpeedActive = !halfSpeedActive;
      MIDI.sendControlChange(CC_HALF_SPEED, MIDI_VALUE, MIDI_CHANNEL);
      digitalWrite(LED_PLAY, halfSpeedActive);
    }
  }
}
