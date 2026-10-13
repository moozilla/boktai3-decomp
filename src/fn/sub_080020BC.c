#include "global.h"
#define Background_LoadMap sub_080020BC
struct Resource {
    u32 header[3], maps, tiles, extra;
};
u8 *sub_08216E54(u16);
s32 Background_FindMapId(struct Resource *, u16);
void sub_082164AC(s32, void *, u16);
s32 Background_LoadMap(s32 layer, u16 resourceId, s32 useIndex, u16 index)
{
    struct Resource resource;
    u8 *base = sub_08216E54(resourceId);
    resource = *(struct Resource *)base;
    resource.maps += (u32)base;
    resource.tiles += (u32)base;
    resource.extra += (u32)base;
    if (!useIndex)
        index = Background_FindMapId(&resource, index);
    sub_082164AC(layer, base, index);
    return index;
}
