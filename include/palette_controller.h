#ifndef PALETTE_CONTROLLER_H
#define PALETTE_CONTROLLER_H
#include "global.h"
struct PaletteWave {
    u16 timer,period;
    s16 value,step;
    u8 phase,unused9,dirty,unusedB;
    u16 unusedC,delay,lowDelay,highDelay;
};
struct PaletteTransition {u8 level,brightness,previousBrightness,mode;u16 *from,*to;};
struct PaletteController {
    u8 task[0x18];u16 flags;
    s8 blend,previousBlend;
    u8 dirty,pending;
    s16 previousState;
    u32 selected;
    struct PaletteWave wave;
    s32 value,start,target,time,duration;
    struct PaletteTransition transition;
    u8 unknown58[0x25C-0x58];
    u16 *source[4],*destination[4],*variants[4];
    u16 palettes[4][256];
    u16 blendBuffer[256],effectBuffer[256];
    u16 *hardware,*current;
};
struct PaletteGameState {u8 unknown[14];u8 state;};
extern struct PaletteGameState gUnk_03005430;
#endif
