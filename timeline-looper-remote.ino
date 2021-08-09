#include "MIDI.h"
#include "Button.h"

#define channel 1
#define value 127
#define levelcc 98

Button btnRecord(87, 2, 7);
Button btnPlay(86, 3, 8);
Button btnStop(85, 5, 10);

Button btnAlt(0, 4, 9);

Button btnReverse(94, 2, 7);
Button btnHalf(95, 3, 8);
Button btnUndo(89, 5, 10);

int loopLevel = 17;
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
        MIDI.sendControlChange(levelcc, loopLevel, channel);
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
        MIDI.sendControlChange(btnRecord.midi, value, channel);
      }
      else if ((btnRecord.active && btnPlay.active) == HIGH) {
        btnRecord.active = LOW;
        MIDI.sendControlChange(btnRecord.midi, value, channel);
      }
      else {
        btnPlay.active = HIGH;
        MIDI.sendControlChange(btnRecord.midi, value, channel);
      }
    }

    if (btnPlay.risingEdge) {
      btnPlay.active = HIGH;
      btnRecord.active = LOW;
      MIDI.sendControlChange(btnPlay.midi, value, channel);
    }

    if (btnStop.risingEdge) {
      btnPlay.active = LOW;
      btnRecord.active = LOW;
      loopLevel = 17;
      fadeTime = 20000;
      MIDI.sendControlChange(btnStop.midi, value, channel);
      MIDI.sendControlChange(levelcc, loopLevel, channel);

      if (btnReverse.active) {
        btnReverse.active = LOW;
        MIDI.sendControlChange(btnReverse.midi, value, channel);
      }

      if (btnHalf.active) {
        btnHalf.active = LOW;
        MIDI.sendControlChange(btnHalf.midi, value, channel);
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
      MIDI.sendControlChange(btnReverse.midi, value, channel);
    }

    if (btnUndo.risingEdge) {
      btnRecord.active = LOW;
      MIDI.sendControlChange(btnUndo.midi, value, channel);
    }

    if (btnHalf.risingEdge) {
      btnHalf.active = !btnHalf.active;
      MIDI.sendControlChange(btnHalf.midi, value, channel);
    }

    btnReverse.updateLed();
    btnUndo.updateLed();
    btnHalf.updateLed();
  }

  btnAlt.updateLed();
}
