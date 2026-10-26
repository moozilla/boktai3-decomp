#include "global.h"
#include "ram.h"

void Effect_Remove(void *);

static inline u8 Tst(u32 *m, s32 i)
{
    if (*m & (1 << i))
        return TRUE;
    return FALSE;
}

struct E {
    u8 a[0x30];
};
struct S {
    u8 pad[0x1C];
    struct E e[12];
    u8 pad2[0x25C - 0x1C - 0x30 * 12];
    u32 mask;
};

u32 BlinkingEffect_DestroyPool(struct S *b)
{
    s32 i = 0;
    u32 *m = &b->mask;
    struct E *e = b->e;
    for (; i < 12; e++, i++) {
        if (Tst(m, i))
            Effect_Remove(e);
    }
    {
        u32 z = 0;
        gSceneStatusIndicatorManager = z;
    }
    return 0;
}
