#include "global.h"
#include "background_controller.h"

s32 Background_LoadMapFromScript(void)
{
    struct BackgroundResource resource;
    u16 layer = sub_0821ABA8('p', 0);
    u16 index;
    u8 *base = sub_08216E54(sub_0821ABA8('d', 0));
    resource = *(struct BackgroundResource *)base;
    resource.maps += (u32)base;
    resource.tiles += (u32)base;
    resource.extra += (u32)base;
    if (!({
            u8 found = FALSE;
            u8 *arg;
            if (Script_SeekToKeyword('i') && (arg = Script_GetPc())) {
                index = sub_0821AA00(arg);
                found = TRUE;
            } else
                index = 0;
            found;
        }) &&
        Script_SeekToKeyword('c')) {
        u8 *p = Script_GetPc();
        if (p)
            index = (u16)Background_FindMapId(&resource, sub_0821AA00(p));
    }
    sub_082164AC(layer, base, index);
    return index;
}
