#include "effects/sprites/sprite_effect_controller.h"
static inline u8 IsUsed(struct SpriteEffectControllerPool *p, u32 index)
{
    u32 mask = 1U << index;
    struct SpriteEffectControllerPool *q;
    q = p;
    if (q->used & mask)
        return 1;
    return 0;
}
s32 SpriteEffectController_DestroyPool(struct SpriteEffectControllerPool *input)
{
    struct SpriteEffectControllerPool *p = input;
    s32 i;
    for (i = 0; i < 24; i++) {
        if (IsUsed(p, i)) {
            struct SpriteEffectController *slot = &p->slots[i];
            if (slot->flags & 8) {
                struct SpriteEffectDrawablePrefix *d = slot->drawable;
                if (d->active) {
                    Sprite_Remove(d);
                    Heap_Free(slot->drawable);
                }
            } else {
                struct SpriteEffectDrawablePrefix *d = slot->drawable;
                if (d->active) {
                    Effect_Remove(d);
                    Heap_Free(slot->drawable);
                }
            }
        }
    }
    gSpriteEffectControllerPool = NULL;
    return 0;
}
