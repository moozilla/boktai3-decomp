// MATCHING_CONCESSION: register-pinning
#include "sprite_effect_controller.h"
#define SpriteEffectController_DestroyPool sub_0806B96C
struct DrawablePrefix {
    u32 flags;
    u8 active;
};
static inline u8 IsUsed(struct SpriteEffectControllerPool *p, u32 index)
{
    u32 mask = 1U << index;
    register struct SpriteEffectControllerPool *q asm("r2");
    q = p;
    if (q->used & mask)
        return 1;
    return 0;
}
s32 SpriteEffectController_DestroyPool(struct SpriteEffectControllerPool *input)
{
    struct SpriteEffectControllerPool *p = input;
    s32 i;
    void **sprite;
    u16 *flags;
    register void **effect asm("r5");
    i = 0;
    sprite = &p->slots[0].drawable;
    flags = &p->slots[0].flags;
    effect = sprite;
    for (; i < 24; i++) {
        if (IsUsed(p, i)) {
            if (*flags & 8) {
                struct DrawablePrefix *d = *sprite;
                if (d->active) {
                    Sprite_Remove(d);
                    Heap_Free(*sprite);
                }
            } else {
                struct DrawablePrefix *d = *effect;
                if (d->active) {
                    Effect_Remove(d);
                    Heap_Free(*effect);
                }
            }
        }
        sprite = (void **)((u8 *)sprite + 0x7c);
        flags = (u16 *)((u8 *)flags + 0x7c);
        effect = (void **)((u8 *)effect + 0x7c);
    }
    {
        register u32 zero asm("r1") = 0;
        gUnk_02000178 = (void *)zero;
    }
    return 0;
}
