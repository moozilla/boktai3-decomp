#ifndef GUARD_PALETTE_CYCLE_H
#define GUARD_PALETTE_CYCLE_H
#include "engine.h"

/* The two endpoint palettes can be live BG colors or the task's saved palette. */
struct PaletteCycleEntry {
    u16 *source, *target, *destination;
    u8 count, step, flags, reverse;
    u8 length[2], hold[2];
    u8 held, unknown15[3];
};
struct PaletteCycle {
    struct Task task;
    u32 entryCount;
    u16 colors[256];
    struct PaletteCycleEntry entries[16];
};
void PaletteCycle_RefreshBasePalette(struct PaletteCycle *);
u32 PaletteCycle_UpdateEntry(struct PaletteCycle *, struct PaletteCycleEntry *);
u32 PaletteCycle_Update(struct PaletteCycle *);
void PaletteCycle_Destroy(void);
s32 PaletteCycle_Initialize(struct PaletteCycle *);
struct PaletteCycle *PaletteCycle_Create(void);
s32 PaletteBlend_AddFromScript(void);
s32 Palette_IsDirty(void);
s32 Div(s32, s32);
#endif
