#include "global.h"
struct G { u8 f[0x3c]; s32 n; };
struct P { u32 fl; u8 f[0x8a - 4]; u16 h; };
extern struct G *gUnk_02000238;
void sub_081A3380(struct P *p)
{
    struct G *g = gUnk_02000238;
    if (p != 0) {
        g->n--;
        p->fl |= 1;
        p->h = 0;
        p->fl &= 0xFFFFF9FF;
    }
}
