#include "bosses/vanargand/vanargand_pair_attack.h"
void Collider_SetPosition(void *, void *, u32);
void Vanargand_UpdatePartCollision(struct VanargandPairAttack *boss, struct VanargandDamagePart *part)
{
    struct VanargandPairAttack *owner = boss;
    struct ContactCollider *collider;
    u32 mask = 4;
    if ((boss->flags & mask) || boss->mode == 1) {
        part->flags |= 1;
        part->damage = 0;
        part->contactFlags = 0;
    }
    if (part->contactCooldown) {
        part->contactCooldown--;
        part->flags |= 1;
    }
    if (part->flashTimer) {
        if (--part->flashTimer == 0) {
            struct SpriteRenderResource *graphics = &part->graphics;
            graphics->paletteKey = 0x72;
            graphics->palette = owner->palettes[3];
        }
    }
    {
        u32 mask = 0x40;
        if (boss->flags & mask) {
            struct MotionTarget *target = &part->target;
            s32 mask = -2;
            target->flags &= mask;
        } else {
            struct MotionTarget *target = &part->target;
            u32 mask = 1;
            target->flags |= mask;
        }
    }
    collider = &part->collider;
    if (part->flags & 1) {
        part->pendingDamage = 0;
        collider->flags |= 4;
        if (part->targetActive == 0) {
            struct MotionTarget *target = &part->target;
            target->flags |= 1;
        }
    } else {
        s32 mask = -5;
        collider->flags = mask & collider->flags;
        Collider_SetPosition(collider, &part->sprite.position, 0);
    }
}
