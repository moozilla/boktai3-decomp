#include "sprite_effect_controller.h"
static inline u8 SpriteEffectController_IsUsed(u32 *m, s32 i)
{
    if (*m & (1 << i))
        return TRUE;
    return FALSE;
}

struct SpriteEffectController *
SpriteEffectController_AllocateSlot(struct SpriteEffectControllerPool *b)
{
    s32 i;
    struct SpriteEffectController *e;
    for (i = 0; i < 24; i++) {
        if (!SpriteEffectController_IsUsed(&b->used, i)) {
            e = &b->slots[i];
            b->used |= 1 << i;
            Memory_Clear(e, 0x7C);
            e->index = i;
            return e;
        }
    }
    return 0;
}
