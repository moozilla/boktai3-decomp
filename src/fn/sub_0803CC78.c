#include "global.h"

struct G { u8 f[0x86c]; u16 a; };
struct S { u8 f[0x5f4]; struct G *g; };
struct Q { u8 f[0xc]; u16 a; };
void sub_0803CC78(struct S *p, struct Q *q)
{
    p->g->a = q->a;
}
