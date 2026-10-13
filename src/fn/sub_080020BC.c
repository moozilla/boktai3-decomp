#include "global.h"
#include "background_controller.h"

s32 sub_080020BC(s32 layer, u16 resourceId, s32 useIndex, u16 index)
{
    struct BackgroundResource resource;
    u8 *base = sub_08216E54(resourceId);
    resource = *(struct BackgroundResource *)base;
    resource.maps += (u32)base;
    resource.tiles += (u32)base;
    resource.extra += (u32)base;
    if (!useIndex)
        index = Background_FindMapId(&resource, index);
    sub_082164AC(layer, base, index);
    return index;
}
