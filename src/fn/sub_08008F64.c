#include "global.h"

struct R { u16 a, b, c; };
struct S { u8 f[6]; u16 x; u8 g[0x18]; struct R *r; u8 h[0x30]; u16 o0; u16 o1; u16 o2; };

void sub_08008F64(struct S *s)
{
    struct R *r = s->r;
    if (r != 0) {
        s->o0 = r->a;
        s->o1 = r->b + s->x;
        s->o2 = r->c;
    }
}
