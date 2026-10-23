#include "global.h"
#include "engine.h"
void memcpy(void *, const void *, u32);
struct P2 {
    u16 x, y;
};
struct T5 {
    struct P2 e[5];
};
extern struct T5 gUnk_0824F9C8;
extern struct T5 gUnk_0824F9DC;
struct V {
    u8 pad0[0x58];
    u8 anchor;
    u8 pad1[0x1f8 - 0x59];
    u8 anim[0x212 - 0x1f8];
    u8 flag;
    u8 pad2[5];
    u16 x;
    u16 y;
    u8 pad3[0x2fc - 0x21c];
    u32 idx;
};
void sub_081C5AF0(struct V *p)
{
    struct T5 a;
    struct T5 b;
    memcpy(&a, &gUnk_0824F9C8, 20);
    b = gUnk_0824F9DC;
    p->x = a.e[p->idx].x;
    {
        u32 i = p->idx << 2;
        u16 *a1 = (u16 *)&a + 1;
        p->y = *(u16 *)((u8 *)a1 + i);
    }
    Anim_LoadKeyframe(p->anim, &p->anchor, b.e[p->idx].x, 0);
    p->flag = 3;
}
