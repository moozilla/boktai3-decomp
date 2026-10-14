#include "global.h"

extern u32 gUnk_03005308;
extern u16 gUnk_0203B400[];
s32 Mod(s32, s32);
u8 *sub_0821E76C(u8 *);

static inline u32 Random(void)
{
    u32 offset;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    offset = gUnk_03005308 * 2;
    return *(u16 *)(offset + (u32)gUnk_0203B400);
}

struct S080B8A58 {
    u8 pad0[2];
    u8 index;
    u8 unk3;
    u8 *data;
    u8 *entry;
};

void sub_080B8A58(u8 *owner)
{
    struct S080B8A58 *p = (struct S080B8A58 *)(owner + 0x448);

    u8 n;
    n = Mod(Random(), *p->data);
    if (n == p->index)
        n = Mod(n + 1, *p->data);
    p->index = n;
    p->unk3 = 0;
    p->entry = sub_0821E76C(p->data) + p->index * 8;
}
