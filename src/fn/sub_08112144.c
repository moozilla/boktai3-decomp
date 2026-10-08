#include "global.h"
void sub_082195E0(void *);
void sub_08112144(u8 *p)
{
    u8 *q = p + 0x24;
    s32 i = 0x16;
    do {
        sub_082195E0(q);
        q += 0x60;
    } while (--i >= 0);
}
