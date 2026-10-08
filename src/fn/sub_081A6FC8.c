#include "global.h"

struct A { u8 f[4]; u8 a; u8 g[0x5f]; u8 b; };
void sub_08214514(void *);

void sub_081A6FC8(struct A *p)
{
    if (p != 0) {
        if (p->b != 0)
            sub_08214514((u8 *)p + 0x60);
        if (p->a != 0)
            sub_08214514(p);
    }
}
