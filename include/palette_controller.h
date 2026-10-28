#ifndef PALETTE_CONTROLLER_H
#define PALETTE_CONTROLLER_H
#include "engine.h"
struct PaletteWave {
    u16 timer, period;
    s16 value, step;
    u8 phase, unused9, dirty, unusedB;
    u16 unusedC, delay, lowDelay, highDelay;
};
struct PaletteSlide {
    s32 value, start, target, time, duration; // eased 0..64 blend value
};
struct PaletteTransition {
    u8 level, brightness, previousBrightness, mode;
    u16 *from, *to;  // palette ranges being cross-faded (set from the variant tables)
    u16 colors[256]; // transition output palette
    u32 callback;    // script handle resumed when the fade finishes
};
enum { PALETTE_SET_SOURCE, PALETTE_SET_DESTINATION, PALETTE_SET_VARIANT };
struct PaletteController {
    u8 task[0x18];
    u16 flags; // 1 transition, 2 oscillate, 8 blend enabled, 0x10 brightness fade, 0x20 (guess)
    s8 blend, previousBlend;
    u8 dirty, pending; // pending: bit 1 rebuild variants, 2 transition, 4 brightness (guess)
    s16 previousState;
    u32 selected;
    struct PaletteWave wave;
    struct PaletteSlide slide;
    struct PaletteTransition transition;
    u16 *colorSets[3][4]; // [PALETTE_SET_*][slot]; variants normally point into palettes
    u16 palettes[4][256];
    u16 blendBuffer[256], effectBuffer[256];
    u16 *hardware, *current;
};
struct PaletteGameState {
    u8 unknown[14];
    u8 state;
};
extern struct PaletteGameState gSunClock;
extern struct PaletteController *gPaletteController;
#endif
