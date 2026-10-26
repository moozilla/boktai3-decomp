#include "transient_sprite_effects.h"
s32 TransientSpriteEffects_Destroy(struct TransientSpriteEffects *pool)
{
    s32 index = 0;
    do {
        struct TransientSpriteEffect *slot = &pool->slots[index];
        if (slot->active)
            Sprite_Remove(&slot->sprite);
        ++index;
    } while (index <= 15);
    return (s32)(gTransientSpriteEffects = NULL);
}
