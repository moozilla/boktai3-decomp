#include "global.h"
struct Registry6A37 {
    u8 pad0[0x1C];
    u16 x, y;
    u8 pad20[0x68 - 0x20];
    s32 timer;
};
extern u16 gUnk_0203B400[];
extern u32 gRandomIndex;
s32 sub_08249350(s32, s32);
void Registry6A37_UpdateBackgroundScroll(struct Registry6A37 *);
void SceneTransition_BeginFirstScroll(struct Registry6A37 *);
// confidence=low: scrolls the background and shakes it by a growing random amount; after 120
// frames starts the first scroll transition.
void sub_081C45E0(struct Registry6A37 *p)
{
    u16 *table;
    u32 *seed;
    u16 index;
    u32 mask;
    s32 timer, range, random;
    Registry6A37_UpdateBackgroundScroll(p);
    table = gUnk_0203B400;
    seed = &gRandomIndex;
    index = *seed + 1;
    mask = 0x3FF;
    index &= mask;
    *seed = index;
    random = table[index];
    random >>= 4;
    timer = p->timer;
    range = (timer >> 4) + 3;
    p->x = sub_08249350(random, range) + 0x15E;
    index = (index + 1) & mask;
    *seed = index;
    random = table[index];
    random >>= 4;
    p->y = sub_08249350(random, range) + 0xC4;
    if (timer > 0x77)
        SceneTransition_BeginFirstScroll(p);
    p->timer++;
}
