#include "global.h"

struct P { u8 f00[0x18]; u8 a[8][0x50]; u8 f298[0x2B8 - 0x298 - 0x0]; };

void sub_08214374(u8 *, s32);
void sub_08214424(u8 *, s32);

void sub_0816310C(u8 *p)
{
    u8 *q = p + 0x18;
    s32 i = 7;
    do {
        sub_08214374(q, 0);
        q += 0x50;
        i--;
    } while (i >= 0);
    sub_08214424(p + 0x2B8, 0);
    *(u8 *)(p + 0x342) = 1;
}
