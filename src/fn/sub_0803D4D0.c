#include "global.h"

void sub_0803D1AC(u8 *, u8 *);
void sub_082223F8(u8 *);
void sub_08219D38(u32);

u32 sub_0803D4D0(u8 *p)
{
    u8 *e = p + 0x44;
    s32 i = 3;
    u32 z;
    do {
        sub_0803D1AC(p, e);
        e += 0x8b0;
        i--;
    } while (i >= 0);
    z = 0;
    sub_082223F8(p + 0x2354);
    sub_08219D38(*(u32 *)(p + 0x20));
    *(u32 *)(p + 0x20) = z;
    *(u32 *)0x02000108 = z;
    return 0;
}
