#include "global.h"

struct A { u8 f[0x38]; u32 fl; };
struct B { u8 f[2]; u8 t; };
void sub_08008A98(struct B *, u32);

void sub_08008ABC(struct A *a, u32 unused, struct B *b)
{
    struct B *c = b;
    if (b->t == 1) {
        u32 m = 4;
        if (a->fl & m)
            sub_08008A98(b, 3);
    } else if (b->t == 2) {
        u32 m = 8;
        if (a->fl & m)
            sub_08008A98(c, 4);
    }
}
