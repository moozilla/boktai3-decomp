#include "global.h"

void sub_08159554(u8 *p, s32 i, s32 add)
{
    u8 *q;
    s32 v;

    if (p != 0) {
        p += 0x450;
        q = p + i;
        v = *q + add;
        if (v > 9) {
            v = 10;
        }
        *q = v;
    }
}
