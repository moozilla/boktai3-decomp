#include "global.h"
#include "engine.h"
#include "render.h"
/* PaletteEffect_StartAnimationOverride: Creates a timed shared palette override for an animation */
#define PaletteEffect_StartAnimationOverride sub_08197AE0

struct AnimData08197AE0 {
    u8 pad0[6];
    u16 id;
    u8 pad8[4];
    u8 *tiles;
};
struct Anim08197AE0 {
    u8 pad0[0xc];
    struct AnimData08197AE0 *data;
};
void *PaletteEffect_Create(void *, void *, u32, s32, s32);
void PaletteEffect_UpdateSharedOverride(void *);
void PaletteEffect_Destroy(void *);

void sub_08197AE0(struct Anim08197AE0 *anim, u32 hold)
{
    void *task = PaletteEffect_Create(anim, NULL, hold, 0, 0);
    if (task) {
        Task_SetCallbacks(task, PaletteEffect_UpdateSharedOverride, PaletteEffect_Destroy);
        {
            u16 id = 0x192;
            u8 *tiles = gPaletteBankData + 0x3240;
            anim->data->id = id;
            anim->data->tiles = tiles;
        }
    }
}
