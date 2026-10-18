#include "global.h"
#include "engine.h"
/* PaletteEffect_StartSpriteRedPulse: Buffers selected sprite palette banks and installs the timed red pulse callback */
#define PaletteEffect_StartSpriteRedPulse sub_08197C58

void *PaletteEffect_Create(void *, void *, u32, s32, s32);
void PaletteEffect_ApplyBufferedPalette(void *, s32);
void PaletteEffect_UpdateRedPulse(void *);
void PaletteEffect_Destroy(void *);

void sub_08197C58(void *sprite, u32 hold, s32 count, s32 start)
{
    void *task = PaletteEffect_Create(NULL, sprite, hold, count, start);
    if (task) {
        PaletteEffect_ApplyBufferedPalette(task, start);
        Task_SetCallbacks(task, PaletteEffect_UpdateRedPulse, PaletteEffect_Destroy);
    }
}
