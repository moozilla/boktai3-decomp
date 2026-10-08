#include "global.h"

struct B { u32 a; u32 b; u32 c; u32 d; u32 e; };
struct S { u8 f[0x18]; u16 fl; u8 g[0x1e]; struct B blk; };
extern struct S *gUnk_02000468;
void sub_08003038(struct S *);

void sub_08003588(u32 a, u32 b, u32 c)
{
    struct S *s = gUnk_02000468;
    if (s != 0) {
        struct B *q = &s->blk;
        q->b = a;
        q->c = b;
        s->blk.a = a;
        q->d = 0;
        q->e = c;
        s->fl |= 0xc;
        sub_08003038(s);
    }
}
