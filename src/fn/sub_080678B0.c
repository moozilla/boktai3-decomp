#include "global.h"

extern u32 gUnk_02000128;
void Sprite_Remove(u8 *);
void Collider_Remove(u8 *);

static inline u8 used(u8 *p, s32 i)
{
    if (*(u32 *)(p + 0x38) & (1 << i))
        return TRUE;
    return FALSE;
}

u32 sub_080678B0(u8 *p)
{
    s32 i;
    for (i = 0; i <= 7; i++) {
        if (used(p, i)) {
            u8 *e = p + (i * 0xac + 0x3c);
            if (e[4] != 0)
                Sprite_Remove(e);
            Collider_Remove(e + 0x50);
        }
    }
    return gUnk_02000128 = 0;
}
