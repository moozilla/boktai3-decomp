#ifndef GUARD_STAGE_RESULTS_H
#define GUARD_STAGE_RESULTS_H

#include "global.h"

/* Tentative: the stage-results screen task (allocated at 0x1214 bytes). */

struct StageResultsSprite {
    u8 pad0[8];
    u32 flags; /* bit 0: hidden */
    u8 padC[0xE];
    u8 visible;
    u8 pad1B[5];
    u16 x, y;
    u8 pad24[0x60 - 0x24];
};

struct StageResultsScriptArgs {
    u32 count : 16;
    u32 pad : 16;
    s32 *values;
};

struct StageResults {
    u8 task[0x18];
    void *layerMap;
    void *layerData;
    u8 *palette;
    struct StageResultsSprite sprites[23];
    u8 animation[0x20];
    u16 spriteIds[23];
    u16 selections[23];
    void *spriteSheet;
    u8 menuPalette[0x968 - 0x944];
    u8 portrait[0x9B0 - 0x968];
    u8 portraitStyle[0xF60 - 0x9B0];
    u8 model[0x11B2 - 0xF60];
    u8 unk11B2;
    u8 pad11B3[5];
    void (*update)(struct StageResults *);
    const u8 *labelScript;
    u8 *summaryScript;
    u8 pad11C4[4];
    u8 state;
    u8 pad11C9;
    u8 entering; /* nonzero until the current state has run once */
    u8 pad11CB;
    s32 textWindows[3];
    s32 targetTimes[12];
    u8 isRecord;
    u8 blinkTimer;
    u16 exitTimer;
    u8 pad120C[4];
    u32 exitScript;
};

static inline s32 StageResults_Clamp(s32 value, s32 max)
{
    if (value > max)
        value = max;
    else if (value < 0)
        value = 0;
    return value;
}

static inline void StageResults_SetUpdate(struct StageResults *results,
                                          void (*update)(struct StageResults *), u8 state)
{
    results->update = update;
    results->entering = 1;
    results->state = state;
}

#endif
