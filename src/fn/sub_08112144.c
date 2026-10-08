#include "global.h"

void sub_082195E0(u8 *);

void sub_08112144(u8 *p)
{
    u8 *q = p + 0x24;
    s32 i;

    for (i = 22; i >= 0; i--) {
        sub_082195E0(q);
        q += 0x60;
    }
}
