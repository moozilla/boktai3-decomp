#include "transient_sprite_effects.h"
struct TransientSpriteEffects *TransientSpriteEffects_Create(void *, u32);
void Sprite_LinkToList(void *, u32);
s32 TransientSpriteEffects_Spawn(u32 kind, u32 options, const struct ContactPosition *position,
                                 const struct ContactPosition *velocity, u32 dragDelay, u32 drag)
{
    s32 index;
    if (!gTransientSpriteEffects && !TransientSpriteEffects_Create(NULL, 0))
        return -1;
    index = 0;
    do {
        struct TransientSpriteEffect *slot = &gTransientSpriteEffects->slots[index];
        struct TransientEffectSprite *sprite = &slot->sprite;
        if (!slot->active) {
            s32 priority;
            slot->active = 1;
            if (options & 4)
                priority = (s32)(options & 0xC000) >> 12;
            else
                priority = 1;
            options &= ~0xC000;
            slot->options = options;
            slot->age = 0;
            slot->dragDelay = dragDelay;
            slot->drag = drag;
            if (velocity)
                slot->velocity = *velocity;
            else {
                struct ContactPosition *destination = &slot->velocity;
                u32 zero;
                zero = 0;
                CpuSet(&zero, destination, 0x05000002);
            }
            AnimData_SelectPaletteBank(&slot->graphics, kind + 0x153);
            sprite->position.vector = *position;
            sprite->priority = priority;
            if (options & 1)
                sprite->flags |= 0x200;
            else
                sprite->flags &= ~0x200;
            if (options & 2)
                sprite->flags |= 16;
            else
                sprite->flags &= ~16;
            Sprite_LinkToList(sprite, 0);
            return 0;
        }
        ++index;
    } while (index <= 15);
    return -1;
}
