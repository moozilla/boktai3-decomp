#include "global.h"
struct E { u8 k; u8 pad[0xf]; u16 id; u8 pad2[0xda]; };
struct G { u8 pad[0x18]; u32 n; struct E *e; };
extern struct G *gUnk_02000034;
u32 sub_0821ABA8(u32, s32);
void sub_08008A98(struct E *, u32);
void sub_080094A0(void)
{
    struct G *g = gUnk_02000034;
    if (g != 0) {
        u32 id = sub_0821ABA8(0x6e, 0);
        s32 m1 = -1;
        s32 v = sub_0821ABA8(0x6d, m1);
        if (v != m1 && v <= 4) {
            struct E *e = g->e;
            u32 i = 0;
            for (; i < g->n; i++, e++) {
                if (e->k == 1 && e->id == id) sub_08008A98(e, v);
            }
        }
    }
}
