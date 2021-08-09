#include "MIDI.h"
#include "Button.h"

#define channel 1
#define value 127
#define levelcc 98

Button record(87, 2, 7);
Button play(86, 3, 8);
Button stop(85, 5, 10);

Button alt(0, 4, 9);

Button reverse(94, 2, 7);
Button half(95, 3, 8);
Button undo(89, 5, 10);

int loopLevel = 17;
unsigned long levelThreshold = 2000;
unsigned long fadeTime = 20000;
unsigned long lastLevelTime = 0;

boolean blinkLED = LOW;

void setup() {
  Serial.begin(31250);
}

void loop() {

  alt.debounce();

  if (flanke(alt)) {
    lastLevelTime = millis();
    alt.state = !alt.state;
    fadeTime = 20000;
    //digitalWrite(alt.led, alt.state);
  }

  if (alt.button && alt.slope && (loopLevel > 1)) {
    if (millis() - lastLevelTime > levelThreshold) {
      alt.state = LOW;
      fadeTime--;
      if ((fadeTime % 2000) == 0) {
        loopLevel--;
        if (loopLevel < 1) {
          //digitalWrite(alt.led, LOW);
          loopLevel = 1;
          goto exitLevel;
        }
        MIDI.sendControlChange(levelcc, loopLevel, channel);
        blinkLED = !blinkLED;
        //digitalWrite(stop.led, blinkLED);
      }
    }
  }

exitLevel:

  // Main block

  if (alt.state == LOW) {

    record.debounce();
    play.debounce();
    stop.debounce();

    if (flanke(record)) {
      if (record.state == LOW) {
        record.state = HIGH;
        MIDI.sendControlChange(record.midi, value, channel);
      }
      else if ((record.state && play.state) == HIGH) {
        record.state = LOW;
        MIDI.sendControlChange(record.midi, value, channel);
      }
      else {
        play.state = HIGH;
        MIDI.sendControlChange(record.midi, value, channel);
      }
    }

    if (flanke(play)) {
      play.state = HIGH;
      record.state = LOW;
      MIDI.sendControlChange(play.midi, value, channel);
    }

    if (flanke(stop)) {
      play.state = LOW;
      record.state = LOW;
      loopLevel = 17;
      fadeTime = 20000;
      MIDI.sendControlChange(stop.midi, value, channel);
      MIDI.sendControlChange(levelcc, loopLevel, channel);

      if (reverse.state) {
        reverse.state = LOW;
        MIDI.sendControlChange(reverse.midi, value, channel);
      }

      if (half.state) {
        half.state = LOW;
        MIDI.sendControlChange(half.midi, value, channel);
      }
    }

    updateLED(record);
    updateLED(play);
    updateLED(stop);

    record.slope = record.button;
    play.slope = play.button;
    stop.slope = stop.button;

  }

  // Alternative block

  else {

    reverse.debounce();
    undo.debounce();
    half.debounce();

    if (flanke(reverse)) {
      reverse.state = !reverse.state;
      MIDI.sendControlChange(reverse.midi, value, channel);
    }

    if (flanke(undo)) {
      record.state = LOW;
      MIDI.sendControlChange(undo.midi, value, channel);
    }

    if (flanke(half)) {
      half.state = !half.state;
      MIDI.sendControlChange(half.midi, value, channel);
    }

    updateLED(reverse);
    updateLED(undo);
    updateLED(half);

    reverse.slope = reverse.button;
    undo.slope = undo.button;
    half.slope = half.button;

  }

  updateLED(alt);
  alt.slope = alt.button;

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
