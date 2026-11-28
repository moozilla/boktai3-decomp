#include "effects/feathers/feather_effect_pool.h"
#include "engine/engine.h"

u8 Sprite_BlinkUntilExpired(void *, u32, s32, s32);

void FadingSprite_UpdateExpiry(struct FeatherPool *unused, struct Feather *feather)
{
    if (feather->enter) {
        feather->enter = 0;
        feather->blinkAge = 0;
    }
    if (Sprite_BlinkUntilExpired(&feather->sprite, feather->blinkAge, 60, 40)) {
        struct FeatherPool *pool = gFadingSpritePool;

        pool->used &= ~(1 << feather->slot);
        feather->state = 0;
        Sprite_Remove(&feather->sprite);
        feather->slot = -1;
        feather->age = 0;
    } else {
        feather->blinkAge++;
    }
}
