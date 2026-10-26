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
    u8 a[0x90];
};
struct S {
    u8 pad[0x18];
    struct E e[8];
    u8 pad2[0x498 - 0x18 - 0x90 * 8];
    u32 mask;
};

u32 FloorSwitch_OnManagerDestroy(struct S *b)
{
    s32 i = 0;
    u32 *m = &b->mask;
    struct E *e = b->e;
    for (; i < 8; e++, i++) {
        if (Tst(m, i))
            Sprite_Remove(e);
    }
    {
        u32 z = 0;
        gFloorSwitchManager = z;
    }
    return 0;
}
