// IO configuration

#define pinBtnRecord 2
#define pinBtnPlay 3
#define pinBtnPage 4
#define pinBtnStop 5

#define ledRecord 7
#define ledPlay 8
#define ledPage 9

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