#include "effects/sprites/transient_sprite_effects.h"
#include "objects/orbit_motion.h"
s32 TransientSpriteEffects_Update(struct TransientSpriteEffects *pool)
{
    s32 index = 0;
    do {
        struct TransientSpriteEffects *view =
            (struct TransientSpriteEffects *)((u8 *)pool + index * sizeof(pool->slots[0]));
        if (view->slots[0].active) {
            struct TransientSpriteEffect *slot;
            struct TransientEffectSprite *sprite;
            // Two steps: the slot is the view advanced past the task header.
            slot = (void *)view;
            slot = (void *)((u8 *)slot + sizeof(view->task));
            sprite = &slot->sprite;
            if (slot->age >= slot->dragDelay) {
                s32 drag;
                s32 x = slot->velocity.x;
                u32 factor;
                factor = slot->drag;
                x *= factor;
                drag = factor;
                slot->velocity.x = scale8(x);
                slot->velocity.y = scale8(slot->velocity.y * drag);
                slot->velocity.z = scale8(slot->velocity.z * drag);
            }
            {
                sprite->position.vector.x += (u16)slot->velocity.x;
                sprite->position.vector.y += (u16)slot->velocity.y;
                if (!(sprite->flags & 16))
                    sprite->position.vector.z += (u16)slot->velocity.z;
                sprite->frame = slot->age >> 2;
                ++slot->age;
                if (slot->age > 24) {
                    slot->active = 0;
                    Sprite_Remove(sprite);
                }
            }
        }
        ++index;
    } while (index <= 15);
    return 0;
}
