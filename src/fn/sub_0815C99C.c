#include "global.h"

struct P {
    u8 f00[0x1c]; s32 st;
    u8 f20[0x457 - 0x20]; u8 c57; u8 c58; u8 c59; u16 h5a;
    u8 f5c[0x479 - 0x45C]; u8 c79;
    u8 f7a[0x59C - 0x47A]; u8 b9c;
};

void sub_0815C99C(struct P *p)
{
    u32 z;
    if (p->c57 != 6)
        p->c57 = 0;
    p->c58 = 0;
    p->h5a = 0;
    z = 0;
    if (p->b9c != 0)
        p->b9c = z;
    p->st = 1;
    p->c79 = z;
}
