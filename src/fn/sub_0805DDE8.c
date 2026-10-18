#include "scene_hud_map.h"
void SceneHud_DrawCenteredMeter(s32 count)
{
    u16 *cell = SceneHud_GetMapCell(0, 5, 2);
    s32 i = 0;
    s32 start = gUnk_0824DEB8[count - 1];
    s32 end = start + count;
    const u16 *empty = gUnk_0824DED0;
    u32 offset = 0;
    const u16 *selected = gUnk_0824DEC0;
    do {
        u16 tile;
        if ((i >= start) && (i < end)) {
            tile = *(const u16 *)(offset + (u32)selected);
        } else {
            tile = *empty;
        }
        do {
            *cell++ = tile;
            empty++;
        } while (0);
        offset += 2;
        i++;
    } while (i <= 7);
}
