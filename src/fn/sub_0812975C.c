#include "global.h"
#include "ram.h"

void Sprite_Remove(void *);

static inline u8 Tst(u32 *m, s32 i)
{
    if (*m & (1 << i))
        return TRUE;
    return FALSE;
}

struct E {
    u8 a[4];
    u8 on;
    u8 b[0xA0 - 5];
};
struct S {
    u8 pad[0x38];
    u32 mask;
    struct E e[20];
};

u32 FeatherEffectPool_DestroyTask(struct S *b)
{
    s32 i = 0;
    struct E *e = b->e;
    for (; i < 20; e++, i++) {
        if (Tst(&b->mask, i)) {
            if (e->on)
                Sprite_Remove(e);
        }
    }
    {
        u32 z = 0;
        gFadingSpritePool = z;
    }
    return 0;
}
