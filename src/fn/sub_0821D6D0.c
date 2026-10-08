#include "global.h"
struct N { u8 p[8]; struct N *f8; struct N *fc; };
struct A { u8 p[0x18]; struct N *f18; };
extern struct A *gUnk_030052F4;
void sub_0821D6D0(struct N *n)
{
    struct N *prev = n->f8;
    struct N *next = n->fc;
    if (prev) prev->fc = next;
    else gUnk_030052F4->f18 = next;
    if (next) next->f8 = prev;
}
