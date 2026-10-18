#include "global.h"
#include "engine.h"
/* PaletteEffect_StartSpriteWhiteFlash: Buffers selected sprite palette banks and installs the white flash callback */
#define PaletteEffect_StartSpriteWhiteFlash sub_08197D94

void *PaletteEffect_Create(void *, void *, u32, s32, s32);
void PaletteEffect_ApplyBufferedPalette(void *, s32);
void PaletteEffect_UpdateWhiteFlash(void *);
void PaletteEffect_Destroy(void *);

void sub_08197D94(void *sprite, s32 count, s32 start)
{
    void *task = PaletteEffect_Create(NULL, sprite, 0, count, start);
    if (task) {
        PaletteEffect_ApplyBufferedPalette(task, start);
        Task_SetCallbacks(task, PaletteEffect_UpdateWhiteFlash, PaletteEffect_Destroy);
    }
}
