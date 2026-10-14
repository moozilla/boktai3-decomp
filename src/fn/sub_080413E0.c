#include "global.h"

s32 sub_08033690(u32, u32, u32, u32);
void sub_08033A5C(s32, u32);
void sub_08041370(u8 *);

s32 sub_080413E0(u8 *p)
{
    s32 *q = (s32 *)(p + 0xf0);
    s32 r;
    s32 i;
    s32 y;
    r = sub_08033690(1, 3, 10, 2);
    *q = r;
    if (r < 0)
        return -1;
    sub_08033A5C(r, 5);
    q = (s32 *)(p + 0xf4);
    r = sub_08033690(1, 6, 0x1c, 6);
    *q = r;
    if (r < 0)
        return -1;
    sub_08033A5C(r, 5);
    i = 2;
    y = 5;
    q = (s32 *)(p + 0xf8);
    do {
        r = sub_08033690(0xd, y, 0x10, 2);
        *q = r;
        if (r < 0)
            return -1;
        sub_08033A5C(r, 1);
        y += 2;
        q++;
        i++;
    } while (i <= 7);
    sub_08041370(p);
}
