#include "global.h"
#include "ram.h"

void Sprite_Remove(void *);

static inline u8 Tst(u32 *m, s32 i)
{
    if (*m & (1 << i))
        return TRUE;
    return FALSE;
}

u32 AnimatedTile_OnManagerDestroy(u8 *p)
{
    s32 i = 0;
    u32 *m = (u32 *)(p + 0x154);
    for (; i < 3; i++) {
        if (Tst(m, i)) {
            u32 o = i * 0x68 + 0x1C;
            u8 *e = p + o;
            Sprite_Remove(e);
        }
    }
    return gNamedEntryPool = 0;
}
