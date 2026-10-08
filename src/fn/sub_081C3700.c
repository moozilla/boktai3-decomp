#include "global.h"

struct S { u8 pad0[0xd8]; u32 d8; u8 pad1[0xf4-0xdc]; u32 f4; u32 f8; };

void sub_081C3700(struct S *p)
{
    if (p) {
        u32 z = 0;
        p->f4 = z;
        p->f8 = z;
        p->d8 = z;
    }
}
