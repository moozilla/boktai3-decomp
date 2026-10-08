#include "global.h"

struct Q { u8 f[0x6c1]; u8 v; };
struct P { u8 f[0x50]; s32 x; u8 g[0x3d0 - 0x54]; struct Q *q; };
void sub_0806FC64(s32, s16 *);
void sub_080B416C(struct P *p)
{
    struct Q *q = p->q;
    s16 s[2];
    s[0] = 6;
    s[1] = p->x;
    sub_0806FC64(0x19, s);
    q->v = 1;
}
