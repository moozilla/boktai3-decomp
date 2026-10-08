#include "global.h"

struct B { u8 f00[0x18]; u16 h18; u8 f1a[2]; u16 h1c; u8 f1e[0x2c - 0x1e]; u8 c2c; u8 f2d[0x34 - 0x2d]; u16 h34; u16 h36; u16 h38; };
struct A { u8 f00[0xf8]; u32 w; };

void sub_08217EEC(struct B *, u32, s32);

void sub_0815F8F8(struct A *a, struct B *b)
{
    { u16 x = b->h34; u16 y = b->h18; b->h18 = x + y; }
    { u16 x = b->h38; u16 y = b->h1c; b->h1c = x + y; }
    b->c2c++;
    if (b->c2c == 5)
        sub_08217EEC(b, a->w, 5);
}
