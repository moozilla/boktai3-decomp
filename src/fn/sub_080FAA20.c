#include "global.h"

struct P { u32 a, b; };
struct S080FAA20 {
    u8 filler0[0x5c]; struct P p;
    u8 filler1[0xf4 * 4 - 0x64]; u8 *q;
};

void sub_080FAA20(struct S080FAA20 *s, struct P *d)
{
    u8 *q = s->q;
    *(struct P *)(q + 0xf04) = s->p;
    *(struct P *)(q + 0xf0c) = *d;
    *(u16 *)(q + 0xea8) = 0;
}
