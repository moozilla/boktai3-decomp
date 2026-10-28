#include "global.h"
#include "ram.h"

u8 *NamedObject_FindByName(u32 key);

u8 *NamedObject_GetPositionByName(u32 key)
{
    u8 *r;

    if (gNamedActorManager && key) {
        r = NamedObject_FindByName(key);
        if (r) {
            return r + 0x10C;
        }
    }
    return 0;
}
