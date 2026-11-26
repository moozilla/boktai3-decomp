#include "global.h"

struct A {
    u8 f[0xf18];
    u32 a;
    u32 b;
    u16 h;
};

void Vanargand_ResetPaletteTransition(struct A *p)
{
    if (p->h != 0) {
        p->a = 0;
        p->h = 0;
        p->b = 0x6f;
    }
}
