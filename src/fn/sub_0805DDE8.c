// MATCHING_CONCESSION: register-pinning
#include "scene_hud_map.h"
#define SceneHud_DrawCenteredMeter sub_0805DDE8
void SceneHud_DrawCenteredMeter(s32 count)
{
    register u16 *cell asm("r2") = sub_0805DD18(0, 5, 2);
    s32 i = 0;
    s32 start = gUnk_0824DEB8[count - 1];
    s32 end = start + count;
    register const u16 *empty asm("r1") = gUnk_0824DED0;
    u32 offset = 0;
    const u16 *selected = gUnk_0824DEC0;
    do {
        u16 tile;
        if (i >= start && i < end)
            tile = *(const u16 *)(offset + (u32)selected);
        else
            tile = *empty;
        *cell++ = tile;
        empty++;
        offset += 2;
        i++;
    } while (i <= 7);
}
