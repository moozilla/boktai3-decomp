#include "scene_hud_map.h"
#define SceneHud_GetMapCell sub_0805DD18
u16 *SceneHud_GetMapCell(u32 layerIndex, u32 x, u32 y)
{
    struct SceneHudMapLayer *layer = (void *)&gUnk_03004C30[layerIndex];
    u8 *base = (u8 *)layer->map;
    return (u16 *)(base + ((x & 31) << 1) + ((y & 31) << 6));
}
