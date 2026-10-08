#include "global.h"
struct A { u8 pad[0x858]; u32 w; };
struct B { u8 pad[0x16c]; s32 k; };
void sub_0803DBB4(struct A *a, struct B *b)
{
    switch (b->k) {
    case 1: a->w |= 0x20; break;
    case 2: a->w |= 0x10; break;
    case 3: a->w |= 0x110; break;
    case 4: a->w |= 0x300; break;
    }
}
