#include "global.h"
extern u32 gUnk_020001A4;
void Sprite_Remove(void *);
void Collider_Remove(void *);

static inline u8 Tst(u32 *m, s32 i)
{
    if (*m & (1 << i))
        return TRUE;
    return FALSE;
}

u32 sub_080FFFE4(u8 *p)
{
    s32 i = 0;
    u32 *m = (u32 *)(p + 0xA3C);
    for (; i < 12; i++) {
        if (Tst(m, i)) {
            u32 o = i * 0xD8 + 0x1C;
            u8 *e;
            e = p + o;
            Sprite_Remove(e);
            e += 0x60;
            Collider_Remove(e);
        }
    }
    return gUnk_020001A4 = 0;
}
