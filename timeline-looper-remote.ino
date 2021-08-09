#include "MIDI.h"
#include "Button.h"

#define channel 1
#define value 127
#define ccStop 85
#define ccPlay 86
#define ccRecord 87
#define ccUndo 89
#define ccReverse 94
#define ccHalf 95
#define ccLevel 98
#define initLoopLevel 17

Button btnRecord(2, 7);
Button btnPlay(3, 8);
Button btnStop(5, 10);
Button btnAlt(4, 9);
Button btnReverse(2, 7);
Button btnHalf(3, 8);
Button btnUndo(5, 10);

int loopLevel = initLoopLevel;
unsigned long levelThreshold = 2000;
unsigned long fadeTime = 20000;
unsigned long lastLevelTime = 0;

boolean blinkLED = LOW;

void setup() {
  Serial.begin(31250);
}

void loop() {

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
        if (loopLevel < 1) {
          loopLevel = 1;
          goto exitLevel;
        }
        MIDI.sendControlChange(ccLevel, loopLevel, channel);
        blinkLED = !blinkLED;
      }
    }
  }

  exitLevel:

  // Page 1

  if (btnAlt.active == LOW) {

    btnRecord.read();
    btnPlay.read();
    btnStop.read();

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
    }

    if (btnPlay.risingEdge) {
      btnPlay.active = HIGH;
      btnRecord.active = LOW;
      MIDI.sendControlChange(ccPlay, value, channel);
    }

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

      if (btnHalf.active) {
        btnHalf.active = LOW;
        MIDI.sendControlChange(ccHalf, value, channel);
      }
    }

    btnRecord.updateLed();
    btnPlay.updateLed();
    btnStop.updateLed();
  }

  // Page 2

  else {

    btnReverse.read();
    btnUndo.read();
    btnHalf.read();

    if (btnReverse.risingEdge) {
      btnReverse.active = !btnReverse.active;
      MIDI.sendControlChange(ccReverse, value, channel);
    }

    if (btnUndo.risingEdge) {
      btnRecord.active = LOW;
      MIDI.sendControlChange(ccUndo, value, channel);
    }

    if (btnHalf.risingEdge) {
      btnHalf.active = !btnHalf.active;
      MIDI.sendControlChange(ccHalf, value, channel);
    }

    btnReverse.updateLed();
    btnUndo.updateLed();
    btnHalf.updateLed();
  }

  btnAlt.updateLed();
}
