#include "MIDI.h"
#include "Button.h"

#define channel 1
#define value 127
#define ccStop 85
#define ccPlay 86
#define ccRecord 87
#define ccUndo 89
#define ccReverse 94
#define ccHalfSpeed 95
#define ccLevel 98
#define initLoopLevel 17

Button btnRecord(2, 7);
Button btnPlay(3, 8);
Button btnStop(5, 10);
Button btnAlt(4, 9);
Button btnReverse(2, 7);
Button btnHalfSpeed(3, 8);
Button btnUndo(5, 10);

int loopLevel = initLoopLevel;
unsigned long levelThreshold = 2000;
unsigned long fadeTime = 20000;
unsigned long lastLevelTime = 0;

void setup() {
  Serial.begin(31250);
}

void loop() {

  // Page selection and looper level
  btnAlt.read();

  if (btnAlt.risingEdge) {
    lastLevelTime = millis();
    btnAlt.active = !btnAlt.active;
    fadeTime = 20000;
  }

  if (btnAlt.buttonState && btnAlt.lastButtonState && (loopLevel > 1)) {
    if (millis() - lastLevelTime > levelThreshold) {
      btnAlt.active = LOW;
      fadeTime--;
      if ((fadeTime % 2000) == 0) {
        loopLevel--;
        if (loopLevel >= 1) {
          MIDI.sendControlChange(ccLevel, loopLevel, channel);
        } else {
          loopLevel = 1;
        }
      }
    }
  }

  // Page 1
  if (btnAlt.active == LOW) {

    // Read buttons
    btnRecord.read();
    btnPlay.read();
    btnStop.read();

    // Record
    if (btnRecord.risingEdge) {
      if (btnRecord.active == LOW) {
        btnRecord.active = HIGH;
        MIDI.sendControlChange(ccRecord, value, channel);
      }
      else if ((btnRecord.active && btnPlay.active) == HIGH) {
        btnRecord.active = LOW;
        MIDI.sendControlChange(ccRecord, value, channel);
      }
      else {
        btnPlay.active = HIGH;
        MIDI.sendControlChange(ccRecord, value, channel);
      }

      btnRecord.updateLed();
    }

    // Play
    if (btnPlay.risingEdge) {
      btnPlay.active = HIGH;
      btnRecord.active = LOW;
      MIDI.sendControlChange(ccPlay, value, channel);
      btnPlay.updateLed();
    }

    // Stop
    if (btnStop.risingEdge) {
      btnPlay.active = LOW;
      btnRecord.active = LOW;
      loopLevel = initLoopLevel; // Reset loop level
      fadeTime = 20000;
      MIDI.sendControlChange(ccStop, value, channel);
      MIDI.sendControlChange(ccLevel, loopLevel, channel);

      if (btnReverse.active) {
        btnReverse.active = LOW;
        MIDI.sendControlChange(ccReverse, value, channel);
      }

      if (btnHalfSpeed.active) {
        btnHalfSpeed.active = LOW;
        MIDI.sendControlChange(ccHalfSpeed, value, channel);
      }

      btnStop.updateLed();
    }
  }

  // Page 2
  else {

    // Read buttons
    btnReverse.read();
    btnUndo.read();
    btnHalfSpeed.read();

    // Reverse
    if (btnReverse.risingEdge) {
      btnReverse.active = !btnReverse.active;
      MIDI.sendControlChange(ccReverse, value, channel);
      btnReverse.updateLed();
    }

    // Undo
    if (btnUndo.risingEdge) {
      btnRecord.active = LOW;
      MIDI.sendControlChange(ccUndo, value, channel);
      btnUndo.updateLed();
    }

    // Half speed
    if (btnHalfSpeed.risingEdge) {
      btnHalfSpeed.active = !btnHalfSpeed.active;
      MIDI.sendControlChange(ccHalfSpeed, value, channel);
      btnHalfSpeed.updateLed();
    }
  }

  btnAlt.updateLed();
}
