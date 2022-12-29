// IO configuration

#ifndef ALT_CONFIG // Use default config for Arduino Nano

#define PIN_BTN_RECORD 2
#define PIN_BTN_PLAY 3
#define PIN_BTN_PAGE 4
#define PIN_BTN_STOP 5

#define LED_RECORD 7
#define LED_PLAY 8
#define LED_PAGE 9

#define PULLUP false
#define INVERT false

#else // Use alternative config

#define PIN_BTN_RECORD D2
#define PIN_BTN_PLAY D3
#define PIN_BTN_PAGE D4
#define PIN_BTN_STOP D1

#define LED_RECORD D6
#define LED_PLAY D7
#define LED_PAGE D0

#define PULLUP true
#define INVERT true

#endif

// MIDI configuration

#define MIDI_CHANNEL 1
#define MIDI_VALUE 127
#define CC_STOP 85
#define CC_PLAY 86
#define CC_RECORD 87
#define CC_UNDO 89
#define CC_REVERSE 94
#define CC_HALF_SPEED 95
#define CC_LEVEL 98

// Misc

#define INIT_LOOP_LEVEL 127
#define INPUT_DELAY 35
#define LONG_PRESS 1000
#define LEVEL_DELAY 80 // 10s to reduce looper level from 100% to 0% (10000 / 127)