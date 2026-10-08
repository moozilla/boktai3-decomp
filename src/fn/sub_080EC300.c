#include "global.h"
struct A { u8 f[8]; u16 v; };
struct B { u32 a; u8 *p; };
u32 sub_080EC300(struct B *b)
{
    struct A *a = (struct A *)(b->p + 0x48);
    if (a->v == 3) return 1; if (a->v == 4) return 0; return a->v;
}
