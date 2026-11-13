#ifndef GUARD_PALETTE_FRAMES_H
#define GUARD_PALETTE_FRAMES_H
#include "engine.h"

/* Seven 16-color frames captured from the background palette; advanced every four ticks. */
struct PaletteFrames {
    struct Task task;
    s32 time, frame;
    u16 colors[256];
};
void PaletteFrames_CaptureColors(struct PaletteFrames *);
s32 PaletteFrames_Update(struct PaletteFrames *);
u32 PaletteFrames_Destroy(void);
s32 PaletteFrames_Initialize(struct PaletteFrames *);
struct PaletteFrames *PaletteFrames_Create(void);
s32 Palette_IsDirty(void);
#endif
