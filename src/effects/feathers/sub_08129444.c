#include "effects/feathers/feather_effect_pool.h"
#include "engine/ram.h"
u8 Sprite_BlinkUntilExpired(void *, u32, s32, s32);
void Sprite_Remove(void *);
void FadingSprite_UpdateExpiry(struct FeatherPool *unused, struct Feather *sprite)
{
    if (sprite->enter) {
        sprite->enter = 0;
        sprite->blinkAge = 0;
    }
    if (Sprite_BlinkUntilExpired(sprite, sprite->blinkAge, 60, 40)) {
        struct FeatherPool *pool = gFadingSpritePool;
        pool->used &= ~(1 << sprite->slot);
        sprite->state = 0;
        Sprite_Remove(sprite);
        sprite->slot = -1;
        sprite->age = 0;
    } else {
        sprite->blinkAge++;
    }
}
