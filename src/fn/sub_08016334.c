#include "global.h"
extern u8 *gUnk_0200006C;
void sub_08016118(u8 *);
u32 sub_08016334(u8 *s)
{
    u8 **gp = &gUnk_0200006C;
    u32 off = 0x24;
    s32 i = 5;
    u8 *e;
    do {
        e = *gp + off;
        if (*e != 0)
            sub_08016118(e);
        off += 0x224;
        i--;
    } while (i >= 0);
    *(u32 *)(s + 0x18) += 1;
    return 0;
}
