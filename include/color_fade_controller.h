#ifndef COLOR_FADE_CONTROLLER_H
#define COLOR_FADE_CONTROLLER_H
#include "global.h"

// One fade channel (guess: blend of a colour into the screen, 0x40 = no fade)
struct ColorFadeChannel {
    u32 mode;   // 0 idle, 1/3 fade in, 2/4 fade out, 5 hold
    u32 color;  // 15-bit target colour
    u32 start;
    u32 level;  // current blend, 0..0x40
    u32 target;
    u16 time;   // frame counter
    u16 shift;  // duration is 1 << shift frames
};

struct ColorFadeController {
    u8 task[0x18]; // task header
    struct ColorFadeChannel channels[2];
    u32 count; // queued commands
    u16 types[8];
    s32 args[8][8];
};

#endif
