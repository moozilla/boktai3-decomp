#include "global.h"
#define Registry6A37_UpdateFirstScrollTransition sub_081C45E0

extern u32 gUnk_03005308;
extern const u16 gUnk_0203B400[];

struct S {
    u8 pad[0x1c];
    s16 x;
    s16 y;
    u8 pad2[0x68 - 0x20];
    s32 timer;
};
void Registry6A37_UpdateBackgroundScroll(struct S *);
s32 sub_08249350(s32, s32);
void sub_081C4440(struct S *);

void sub_081C45E0(struct S *p)
{
    s32 timer;
    s32 shift;
    const u16 *table;
    u32 *indexPtr;
    u32 index;
    u32 mask;
    s32 n;

    Registry6A37_UpdateBackgroundScroll(p);

    table = gUnk_0203B400;
    indexPtr = &gUnk_03005308;
    index = *indexPtr;
    index++;
    mask = 0x3FF;
    index &= mask;
    *indexPtr = index;
    n = table[index];
    n >>= 4;
    timer = p->timer;
    shift = (timer >> 4) + 3;
    p->x = sub_08249350(n, shift) + 0x15e;

    index = (index + 1) & mask;
    *indexPtr = index;
    n = table[index];
    n >>= 4;
    p->y = sub_08249350(n, shift) + 0xc4;

    if (timer > 0x77) {
        sub_081C4440(p);
    }
    p->timer++;
}
