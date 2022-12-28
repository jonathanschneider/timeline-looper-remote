#include <Arduino.h>
#include <MIDI.h>
#include <EasyButton.h>
#include "config.h"

MIDI_CREATE_DEFAULT_INSTANCE();

EasyButton btnRecord(pinBtnRecord, inputDelay, true, true);
EasyButton btnPlay(pinBtnPlay, inputDelay, true, true);
EasyButton btnPage(pinBtnPage, inputDelay, true, true);
EasyButton btnStop(pinBtnStop, inputDelay, true, true);

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
    if (btnRecord.wasPressed()) {
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
    } else if (recording && recordLongPress && btnRecord.wasReleased()) {

      if (!playing) { // Only recording
        MIDI.sendControlChange(ccPlay, value, channel); // Send play to stop recording and start playing
        playing = true;
        digitalWrite(ledPlay, playing);
      } else { // Recording and playing
        MIDI.sendControlChange(ccRecord, value, channel); // Send record to stop recording and not retrigger loop
      }

      recording = false;
      recordLongPress = false; // Reset long press
      digitalWrite(ledRecord, recording);
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
