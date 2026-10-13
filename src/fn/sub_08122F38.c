#include "global.h"
struct E {
    u8 f[0x42];
    u8 a;
    s8 b;
};
struct G {
    u8 f[0x920];
    u32 m;
};
void Effect_Remove(void *);
void Memory_Clear(void *, u32);
void sub_08122F38(struct G *g, struct E *e)
{
    u8 *p;
    s32 k;
    Effect_Remove(e);
    e->a = 0;
    p = (u8 *)&e->b;
    k = *(s8 *)p;
    g->m &= ~(1 << k);
    Memory_Clear(e, 0x48);
    *p = 0xff;
}
