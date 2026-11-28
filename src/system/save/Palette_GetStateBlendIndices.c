#include "global.h"
#include "effects/palette/palette_controller.h"
extern const u8 gUnk_08E8795C[][2];
s32 Palette_GetStateBlendIndices(u16 *from, u16 *to)
{
    *from = gUnk_08E8795C[gSunClock.state][0];
    *to = gUnk_08E8795C[gSunClock.state][1];
    return 0;
}
