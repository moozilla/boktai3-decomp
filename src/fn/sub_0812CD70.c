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
    u8 a[0xC0];
};
struct S {
    u8 pad[0x38];
    u32 mask;
    struct E e[8];
};

u32 FragmentEffect_DestroyManager(struct S *b)
{
    s32 i = 0;
    struct E *e = b->e;
    for (; i < 8; e++, i++) {
        if (Tst(&b->mask, i))
            Sprite_Remove(e);
    }
    {
        u32 z = 0;
        gFragmentEffectManager = z;
    }
    return 0;
}
