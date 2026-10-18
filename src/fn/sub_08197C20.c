#include "global.h"
#include "engine.h"
/* PaletteEffect_StartAnimationRedPulse: Buffers an animation palette and installs the timed red pulse callback */
#define PaletteEffect_StartAnimationRedPulse sub_08197C20

void *PaletteEffect_Create(void *, void *, u32, s32, s32);
void PaletteEffect_ApplyBufferedPalette(void *, s32);
void PaletteEffect_UpdateRedPulse(void *);
void PaletteEffect_Destroy(void *);

void sub_08197C20(void *anim, u32 hold, s32 count)
{
    void *task = PaletteEffect_Create(anim, NULL, hold, count, 0);
    if (task) {
        PaletteEffect_ApplyBufferedPalette(task, 0);
        Task_SetCallbacks(task, PaletteEffect_UpdateRedPulse, PaletteEffect_Destroy);
    }
}
