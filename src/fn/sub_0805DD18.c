#include "scene_hud_map.h"
#include "render.h"
u16 *SceneHud_GetMapCell(u32 layerIndex, u32 x, u32 y)
{
    struct BackgroundLayer *layer = &gBgRecords[layerIndex];
    u8 *base = (u8 *)layer->map;
    return (u16 *)(base + ((x & 31) << 1) + ((y & 31) << 6));
}
