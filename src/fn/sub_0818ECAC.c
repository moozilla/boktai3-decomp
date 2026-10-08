#include "global.h"
struct E { u32 a; u32 b; u8 f[0x6c-8]; };
struct F { u32 a; u32 b; u8 f[0x188-8]; };
void sub_0824923C(void *, u32);
void sub_0818ECAC(u8 *p)
{
    struct E *e;
    struct F *g;
    s32 i;
    e = (struct E *)p;
    i = 0x18;
    do {
        e->a = 0;
        if (e->b != 0) { sub_0824923C(e, e->b); e->b = 0; }
        i--;
        e++;
    } while (i > 0);
    g = (struct F *)(p + 0xA24);
    i = 0x30;
    do {
        g->a = 0;
        if (g->b != 0) { sub_0824923C(g, g->b); g->b = 0; }
        i--;
        g++;
    } while (i > 0);
}
