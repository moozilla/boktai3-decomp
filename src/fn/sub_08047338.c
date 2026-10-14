// MATCHING_CONCESSION: register-pinning
#include "transient_sprite_effects.h"
static inline s32 effectScale(s32 value)
{
    if (value >= 0)
        return value >> 8;
    return -((-value) >> 8);
}
s32 TransientSpriteEffects_Update(struct TransientSpriteEffects *pool)
{
    s32 index = 0;
    do {
        register u32 offset asm("r0") = index * sizeof(pool->slots[0]);
        register struct TransientSpriteEffects *view asm("r1");
        view = (struct TransientSpriteEffects *)((u32)pool + offset);
        if (view->slots[0].active) {
            struct TransientSpriteEffect *slot = &view->slots[0];
            struct TransientEffectSprite *sprite = &slot->sprite;
            if (slot->age >= slot->dragDelay) {
                s32 drag;
                s32 x = slot->velocity.x;
                u32 factor;
                factor = slot->drag;
                x *= factor;
                drag = factor;
                slot->velocity.x = effectScale(x);
                slot->velocity.y = effectScale(slot->velocity.y * drag);
                slot->velocity.z = effectScale(slot->velocity.z * drag);
            }
            {
                u32 zero;
                u32 x = (u16)slot->velocity.x;
                register u32 delta asm("r1") = (u16)sprite->position.vector.x;
                x += delta;
                zero = 0;
                sprite->position.vector.x = x;
                sprite->position.vector.y += (u16)slot->velocity.y;
                if (!(sprite->flags & 16))
                    sprite->position.vector.z += (u16)slot->velocity.z;
                sprite->frame = slot->age >> 2;
                ++slot->age;
                if (slot->age > 24) {
                    slot->active = zero;
                    Sprite_Remove(sprite);
                }
            }
        }
        ++index;
    } while (index <= 15);
    return 0;
}
