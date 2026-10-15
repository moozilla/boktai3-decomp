#include "scene_hud_map.h"
void SceneHud_FillTile001(u32 x, u32 y, s32 count)
{
    u16 *cell = SceneHud_GetMapCell(0, x, y);
    if (count > 0) {
        do {
            *cell++ = 0xF001;
            count--;
        } while (count > 0);
    }
}
