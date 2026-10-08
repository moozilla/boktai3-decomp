#include "global.h"

struct P { u8 f00[0x34]; u32 w34; u8 f38[0x60 - 0x38]; u32 w60; };

void sub_08160640(struct P *p)
{
    if (p != 0) {
        u32 m = ~1;
        p->w60 &= m;
        p->w34 &= m;
    }
}
