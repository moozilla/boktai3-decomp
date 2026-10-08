#include "global.h"

extern void *gUnk_020001AC;

u8 *sub_08101A3C(u32 key);

u8 *sub_08103EF4(u32 key)
{
    u8 *r;

    if (gUnk_020001AC && key) {
        r = sub_08101A3C(key);
        if (r) {
            return r + 0x10C;
        }
    }
    return 0;
}
