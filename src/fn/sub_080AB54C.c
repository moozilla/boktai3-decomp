#include "global.h"

struct Q { u8 f[0x878]; u8 n; u8 pad; u16 pad2; u16 v[4]; };
struct P { u8 f[0x3d0]; struct Q *q; };

void sub_080AB54C(struct P *p, u16 x)
{
    struct Q *q = p->q;
    q->v[q->n] = x;
    q->n = (q->n + 1) & 3;
}
