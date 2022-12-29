#define ARDUINO_NANO 0
#define NODEMCU 1

#ifndef CONFIG
#define CONFIG ARDUINO_NANO // Arduino Nano is default config
#endif

// IO configuration

#if CONFIG == ARDUINO_NANO // Default config for Arduino Nano

#define pinBtnRecord 2
#define pinBtnPlay 3
#define pinBtnPage 4
#define pinBtnStop 5

#define ledRecord 7
#define ledPlay 8
#define ledPage 9

#define PULLUP false
#define INVERT false

#elif CONFIG == NODEMCU // NodeMCU

#define pinBtnRecord D2
#define pinBtnPlay D3
#define pinBtnPage D4
#define pinBtnStop D1

#define ledRecord D6
#define ledPlay D7
#define ledPage D0

#define PULLUP true
#define INVERT true

#endif

// MIDI configuration

#define channel 1
#define value 127
#define ccStop 85
#define ccPlay 86
#define ccRecord 87
#define ccUndo 89
#define ccReverse 94
#define ccHalfSpeed 95
#define ccLevel 98

// Misc

#define initLoopLevel 127
#define inputDelay 35

const unsigned short LONG_PRESS = 1000;
const unsigned short LEVEL_DELAY = 80; // 10s to reduce looper level from 100% to 0% (10000 / 127)