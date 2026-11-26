#ifndef GUARD_TIME_PALETTE_H
#define GUARD_TIME_PALETTE_H
#include "global.h"

/* A callback-driven transition between three time-of-day palette banks. */
struct TimePalette {
    void (*update)(struct TimePalette *);
    void (*onComplete)(struct TimePalette *);
    const u16 *sources[3];
    u8 from, to, phase, timer;
    u16 colors[16];
};

s32 TimePalette_GetSourceIndex(u32 phase);
void TimePalette_UpdateTransition(struct TimePalette *palette);
void TimePalette_InitTransition(struct TimePalette *palette, const u16 *phase12, const u16 *phase3,
                                const u16 *otherPhases);
#endif
