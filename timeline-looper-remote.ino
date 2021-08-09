#include <MIDI.h>
#include "Button.h"

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
#define initLoopLevel 17

const unsigned short LEVEL_DELAY = 2000;

Button btnRecord(2, 7);
Button btnPlay(3, 8);
Button btnStop(5, 10);
Button btnAlt(4, 9);
Button btnReverse(2, 7);
Button btnHalfSpeed(3, 8);
Button btnUndo(5, 10);

unsigned short loopLevel = initLoopLevel;
unsigned short levelTimer = 0;

void setup() {
  Serial.begin(31250);
}

void loop() {

  // Page selection and looper level
  btnAlt.read();

  if (btnAlt.risingEdge == true) {
    levelTimer = millis();
    btnAlt.active = !btnAlt.active;
  }

  if (btnAlt.longPressActive == true && loopLevel > 0) {
    if (millis() - levelTimer > LEVEL_DELAY) {
      levelTimer = millis(); // Reset timer
      btnAlt.active = false;
      loopLevel--;
      MIDI.sendControlChange(ccLevel, loopLevel, channel);
    }
  }

  // Page 1
  if (btnAlt.active == false) {

    // Read buttons
    btnRecord.read();
    btnPlay.read();
    btnStop.read();

    // Record
    if (btnRecord.risingEdge == true) {
      if (btnRecord.active == false) {
        btnRecord.active = true;
        MIDI.sendControlChange(ccRecord, value, channel);
      }
      else if ((btnRecord.active && btnPlay.active) == true) {
        btnRecord.active = false;
        MIDI.sendControlChange(ccRecord, value, channel);
      }
      else {
        btnPlay.active = true;
        MIDI.sendControlChange(ccRecord, value, channel);
      }

      btnRecord.updateLed();
    }
    else if (btnRecord.longPressReleased == true && btnRecord.active == true) {
        btnRecord.active = false;
        MIDI.sendControlChange(ccRecord, value, channel);
    }

    // Play
    if (btnPlay.risingEdge == true) {
      btnPlay.active = true;
      btnRecord.active = false;
      MIDI.sendControlChange(ccPlay, value, channel);
      btnPlay.updateLed();
    }

    // Stop
    if (btnStop.risingEdge == true) {
      btnPlay.active = false;
      btnRecord.active = false;
      loopLevel = initLoopLevel; // Reset loop level
      MIDI.sendControlChange(ccStop, value, channel);
      MIDI.sendControlChange(ccLevel, loopLevel, channel);

      if (btnReverse.active == true) {
        btnReverse.active = false;
        MIDI.sendControlChange(ccReverse, value, channel);
      }

      if (btnHalfSpeed.active == true) {
        btnHalfSpeed.active = false;
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
    if (btnReverse.risingEdge == true) {
      btnReverse.active = !btnReverse.active;
      MIDI.sendControlChange(ccReverse, value, channel);
      btnReverse.updateLed();
    }

    // Undo
    if (btnUndo.risingEdge == true) {
      btnRecord.active = false;
      MIDI.sendControlChange(ccUndo, value, channel);
      btnUndo.updateLed();
    }

    // Half speed
    if (btnHalfSpeed.risingEdge == true) {
      btnHalfSpeed.active = !btnHalfSpeed.active;
      MIDI.sendControlChange(ccHalfSpeed, value, channel);
      btnHalfSpeed.updateLed();
    }
  }

  btnAlt.updateLed();
}
