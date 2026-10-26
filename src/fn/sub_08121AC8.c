#include "global.h"
#include "ram.h"

void Sprite_Remove(void *);

static inline u8 Tst(u32 *m, s32 i)
{
    if (*m & (1 << i))
        return TRUE;
    return FALSE;
}

u32 SceneColliderEffect_DestroyPool(u8 *p)
{
    s32 i = 0;

    for (; i < 4; i++) {
        if (Tst((u32 *)(p + 0x38), i)) {
            u32 o = i * 0xA4 + 0x3C;
            u8 *e = p + o;
            Sprite_Remove(e);
        }
    }
    return gColliderEffectPool = 0;
}
