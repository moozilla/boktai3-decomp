#include "transient_sprite_effects.h"
void Sprite_InitUnlinked(void *, void *, u32);
s32 TransientSpriteEffects_Initialize(struct TransientSpriteEffects *pool, void *context)
{
    s32 index;
    struct ContactPackedVector position;
    gTransientSpriteEffects = pool;
    position.x = 0;
    position.y = 0;
    position.z = 0;
    for (index = 0; index <= 15; index++) {
        struct TransientSpriteEffect *slot = &pool->slots[index];
        struct ContactSwarmGraphics *graphics;
        slot->active = 0;
        graphics = &slot->graphics;
        if (!AnimData_Load(graphics, 0x8639))
            return -1;
        Sprite_InitUnlinked(&slot->sprite, graphics, 0);
        slot->sprite.priority = 1;
        slot->sprite.position.bits = position;
    }
    return 0;
}
