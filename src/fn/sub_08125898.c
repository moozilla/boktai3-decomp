#include "global.h"
struct E { u8 f[0xde]; s8 b; u8 a; };
struct G { u8 f[0x1e0]; u32 m; };
void sub_08003C10(void *);
void sub_08225938(void *);
void sub_08219DD8(void *, u32);
void sub_08125898(struct G *g, struct E *e)
{
    u8 *p;
    s32 k;
    e->a = 0;
    p = (u8 *)&e->b;
    k = *(s8 *)p;
    g->m &= ~(1 << k);
    sub_08003C10(e);
    sub_08225938(e);
    sub_08219DD8(e, 0xe4);
    *p = 0xff;
}
