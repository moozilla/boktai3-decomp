#define gPaletteStateBlendIndices gUnk_08E8795C
#include "global.h"
#include "palette_controller.h"
extern const u8 gUnk_08E8795C[];
s32 Palette_GetStateBlendIndices(u16 *from, u16 *to)
{
    const u8 *pairs = gUnk_08E8795C;
    struct PaletteGameState *state = &gSunClock;
    *from = *(u8 *)(state->state * 2 + (u32)pairs);
    {
        u32 offset = state->state * 2;
        pairs++;
        *to = *(u8 *)(offset + (u32)pairs);
    }
    return 0;
}
