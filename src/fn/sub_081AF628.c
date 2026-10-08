#include "global.h"

struct A { u8 f[0x18]; u8 g[4]; u8 a; };
void sub_08214514(void *);

void sub_081AF628(struct A *p)
{
    if (p->a != 0)
        sub_08214514(p->g);
}
