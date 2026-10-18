#include "global.h"
#include "engine.h"
/* PaletteEffect_StartAnimationWhiteFlash: Buffers an animation palette and installs the white flash callback */
#define PaletteEffect_StartAnimationWhiteFlash sub_08197D5C

void *PaletteEffect_Create(void *, void *, u32, s32, s32);
void PaletteEffect_ApplyBufferedPalette(void *, s32);
void PaletteEffect_UpdateWhiteFlash(void *);
void PaletteEffect_Destroy(void *);

void sub_08197D5C(void *anim, s32 count)
{
    void *task = PaletteEffect_Create(anim, NULL, 0, count, 0);
    if (task) {
        PaletteEffect_ApplyBufferedPalette(task, 0);
        Task_SetCallbacks(task, PaletteEffect_UpdateWhiteFlash, PaletteEffect_Destroy);
    }
}
