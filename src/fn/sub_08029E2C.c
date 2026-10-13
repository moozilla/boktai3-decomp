#include "global.h"
extern u32 gUnk_03005308;
extern u16 gUnk_0203B400[];
int sub_08029E2C(u8 *p)
{
    int start, i, index;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    {
        u32 offset = gUnk_03005308 * 2;
        {
            int value = *(u16 *)((u8 *)gUnk_0203B400 + offset);
            value &= 3;
            start = value;
        }
    }
    for (i = start; i < start + 4; i++) {
        index = i & 3;
        if (!((p[0x1a3] >> index) & 1)) {
            p[0x1a3] |= 1 << index;
            goto done;
        }
    }
    index = start;
    {
        int mask = 1 << index;
        p[0x1a3] = mask;
    }
done:
    return index + 4;
}
