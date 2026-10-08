#include "global.h"

struct P0814AFE4 { u8 f0[0x3A2]; u8 f3A2; u8 f3A3; };

void sub_0814AFE4(struct P0814AFE4 *p, s32 v)
{
    s32 k = ((v + 0x60) & 0xff) >> 6;
    if (k > 1) {
        p->f3A2 = 3 - k;
        p->f3A3 = 1;
    } else {
        p->f3A2 = k;
        p->f3A3 = 0;
    }
}
