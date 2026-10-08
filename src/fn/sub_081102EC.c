#include "global.h"
struct G { u8 f[0x1c]; u8 b; };
struct S { u8 f[0xFE4]; void *fp; u8 p[0x18]; u8 st; u8 q[0x23]; u8 c; };
extern struct G *gUnk_020004B4;
void sub_08110334(void);
static inline void set(struct S *s, void *fp, u8 st) { s->fp = fp; s->c = 1; s->st = st; }
void sub_081102EC(struct S *s)
{
    struct G *g = gUnk_020004B4;
    s32 v = -1;
    if (g != 0)
        v = g->b;
    if (v == 0x1b) {
        set(s, sub_08110334, 6);
    }
}
