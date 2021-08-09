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

  btnAlt.debounce();

  if (flanke(btnAlt)) {
    lastLevelTime = millis();
    btnAlt.state = !btnAlt.state;
    fadeTime = 20000;
  }

  if (btnAlt.button && btnAlt.slope && (loopLevel > 1)) {
    if (millis() - lastLevelTime > levelThreshold) {
      btnAlt.state = LOW;
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

  // Main block

  if (btnAlt.state == LOW) {

    btnRecord.debounce();
    btnPlay.debounce();
    btnStop.debounce();

    if (flanke(btnRecord)) {
      if (btnRecord.state == LOW) {
        btnRecord.state = HIGH;
        MIDI.sendControlChange(btnRecord.midi, value, channel);
      }
      else if ((btnRecord.state && btnPlay.state) == HIGH) {
        btnRecord.state = LOW;
        MIDI.sendControlChange(btnRecord.midi, value, channel);
      }
      else {
        btnPlay.state = HIGH;
        MIDI.sendControlChange(btnRecord.midi, value, channel);
      }
    }

    if (flanke(btnPlay)) {
      btnPlay.state = HIGH;
      btnRecord.state = LOW;
      MIDI.sendControlChange(btnPlay.midi, value, channel);
    }

    if (flanke(btnStop)) {
      btnPlay.state = LOW;
      btnRecord.state = LOW;
      loopLevel = 17;
      fadeTime = 20000;
      MIDI.sendControlChange(btnStop.midi, value, channel);
      MIDI.sendControlChange(levelcc, loopLevel, channel);

      if (btnReverse.state) {
        btnReverse.state = LOW;
        MIDI.sendControlChange(btnReverse.midi, value, channel);
      }

      if (btnHalf.state) {
        btnHalf.state = LOW;
        MIDI.sendControlChange(btnHalf.midi, value, channel);
      }
    }

    updateLED(btnRecord);
    updateLED(btnPlay);
    updateLED(btnStop);

    btnRecord.slope = btnRecord.button;
    btnPlay.slope = btnPlay.button;
    btnStop.slope = btnStop.button;

  }

  // Page 2

  else {

    btnReverse.debounce();
    btnUndo.debounce();
    btnHalf.debounce();

    if (flanke(btnReverse)) {
      btnReverse.state = !btnReverse.state;
      MIDI.sendControlChange(btnReverse.midi, value, channel);
    }

    if (flanke(btnUndo)) {
      btnRecord.state = LOW;
      MIDI.sendControlChange(btnUndo.midi, value, channel);
    }

    if (flanke(btnHalf)) {
      btnHalf.state = !btnHalf.state;
      MIDI.sendControlChange(btnHalf.midi, value, channel);
    }

    updateLED(btnReverse);
    updateLED(btnUndo);
    updateLED(btnHalf);

    btnReverse.slope = btnReverse.button;
    btnUndo.slope = btnUndo.button;
    btnHalf.slope = btnHalf.button;

  }

  updateLED(btnAlt);
  btnAlt.slope = btnAlt.button;

}

bool flanke(Button o) {
  return (o.button == HIGH && o.slope == LOW);
}

bool negflanke(Button o) {
  return (o.button == LOW && o.slope == HIGH);
}

void updateLED(Button o) {
  digitalWrite(o.led, o.state);
}
