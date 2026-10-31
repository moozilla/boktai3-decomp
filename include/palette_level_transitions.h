#ifndef GUARD_PALETTE_LEVEL_TRANSITIONS_H
#define GUARD_PALETTE_LEVEL_TRANSITIONS_H

#include "engine.h"

/* Historical PaletteLevels names refer to four packed hardware mosaic sizes,
 * not color brightness. Channels are BG horizontal/vertical, OBJ horizontal/vertical
 * (tentative interpretation of the Mosaic_SetParameters consumer).
 */
struct PaletteLevels {
    struct Task task;         /* 0x00 */
    u8 state;                 /* 0x18: index into the update callback table */
    u8 first, count;          /* 0x19: legacy names; mosaic target selectors (tentative) */
    u8 enabled;               /* 0x1B: active channel mask */
    u16 timers[4];            /* 0x1C */
    u16 periods[4];           /* 0x24: frames per step */
    s8 current[4], target[4]; /* 0x2C, 0x30: 0..15 */
};

/* 0x02000070 */
extern struct PaletteLevels *gPaletteLevels;

void PaletteLevels_Increase(struct PaletteLevels *p);
void PaletteLevels_Decrease(struct PaletteLevels *p);
s32 PaletteLevels_Update(struct PaletteLevels *p);
u32 PaletteLevels_Destroy(void);
/* Preserve the original consumed-return ABI: Initialize and Update fall through.
 * Create forwards its first argument as a halfword; Configure also passes a
 * second unused argument. Keep those call-site arguments for exact matching. */
s32 PaletteLevels_Initialize(struct PaletteLevels *p, u16 unused);
struct PaletteLevels *PaletteLevels_Create(u32 arg, u32 unused);
s32 PaletteLevels_Configure(s32 mode, s32 first, s32 count, u8 *initial, u8 *target, u16 *periods);
s32 PaletteLevels_ConfigureFromScript(void);
void PaletteLevels_Reset(void);

#endif
