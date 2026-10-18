#include "global.h"

extern void *gUnk_020001AC;

u8 *NamedObject_FindByName(u32 key);

u8 *NamedObject_GetPositionByName(u32 key)
{
    u8 *r;

    if (gUnk_020001AC && key) {
        r = NamedObject_FindByName(key);
        if (r) {
            return r + 0x10C;
        }
    }
    return 0;
}
