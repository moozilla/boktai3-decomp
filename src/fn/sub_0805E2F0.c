#include "global.h"

void sub_082195E0(u8 *);

void sub_0805E2F0(u8 *p)
{
    u8 *q = p + 0x20;
    s32 i;

    for (i = 1; i >= 0; i--) {
        sub_082195E0(q);
        q += 0x60;
    }
}
