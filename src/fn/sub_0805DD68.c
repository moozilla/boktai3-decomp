#include "scene_hud_map.h"
#define SceneHud_FillTile020 sub_0805DD68
void SceneHud_FillTile020(u32 x, u32 y, s32 count)
{
    u16 *cell = sub_0805DD18(0, x, y);
    s32 remaining;
    for (remaining = count; remaining > 0; remaining--)
        *cell++ = 0xF020;
}
