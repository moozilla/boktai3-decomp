#include "global.h"

extern const u32 gUnk_08605050[];

int sub_0802D334(u8 *p)
{
    u16 *idp = (u16 *)(p + 0x12e);
    u32 v = *idp;
    u32 id;
    if (v == 0)
        return 0;
    id = v;
    {
        u32 zero;
        u16 *ip = (u16 *)(p + 0x12a);
        *(u32 *)(p + 0x11c) = gUnk_08605050[id];
        zero = 0;
        *ip = id;
        *(u8 *)(p + 0x12d) = 1;
        *(u32 *)(p + 0x134) = zero;
    }
    return 1;
}
