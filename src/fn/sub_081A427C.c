#include "global.h"
struct G { u8 f[0x58]; u8 n; };
struct P { u32 fl; u8 f[0x50 - 4]; u16 h; };
extern struct G *gUnk_02000240;
void sub_081A427C(struct P *p)
{
    struct G *g = gUnk_02000240;
    if (p != 0) {
        g->n--;
        p->h = 0;
        p->fl |= 1;
    }
}
