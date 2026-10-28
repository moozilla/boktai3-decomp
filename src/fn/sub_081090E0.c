#include "global.h"
#include "area_effect_link.h"
struct Sub {
    u8 pad[0x84];
    s8 slots[5];
};
struct S {
    u8 pad[0x53];
    u8 busy;
    u8 pad54[0xe0 - 0x54];
    struct Sub sub;
};

static inline s32 AddSlot(struct Sub *q, s32 v)
{
    s32 i;
    if (!q)
        return -1;
    for (i = 0; i <= 4; i++) {
        if (q->slots[i] == -1) {
            q->slots[i] = v;
            return 1;
        }
    }
    return 0;
}
s32 sub_081090E0(s32 v)
{
    struct S *s = gLinkSession;
    if (!s || s->busy)
        return -1;
    return AddSlot(&s->sub, v);
}
