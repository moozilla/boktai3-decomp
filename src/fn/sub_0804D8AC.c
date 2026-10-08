#include "global.h"
struct E { u8 a[6]; u8 t; };
struct L { u8 pad[6]; u8 n[2]; u32 e[2][4]; };
struct G { u8 pad[0x24]; u8 idx; };
extern struct G *gUnk_0200047C;
void sub_08020D54(struct L *, struct E *);
s32 sub_0804D8AC(u8 *p)
{
    struct L *l = (struct L *)(p + 0x18);
    s32 i = 0;
    if (i < l->n[gUnk_0200047C->idx]) {
        do {
            struct G *g = gUnk_0200047C;
            struct E *e = (struct E *)l->e[g->idx][i];
            sub_08020D54(l, e);
            if (e->t == 3) return 1;
            i++;
        } while (i < l->n[gUnk_0200047C->idx]);
    }
    return 0;
}
