#include "global.h"

#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Remaining: loop induction and collider pointer lifetimes.
#define gVanargandProjectileProperties gUnk_08612114
struct ProjectilePosition {
    s16 x, y, z, pad;
};
struct ProjectileSprite {
    u32 flags;
    u8 pad4[0x18];
    struct ProjectilePosition position;
    u8 tail[8];
};
struct ProjectileCollider {
    u8 pad0[6];
    u16 flags;
    u16 pad8, extent;
    u8 tail[0x48];
};
struct VanargandProjectile {
    u8 entered, state, type;
    u8 pad3[3];
    u16 timer;
    u8 trailIndex, direction;
    u16 targetAngle, angle, steeringTimer, speed;
    u8 pad12[2];
    struct ProjectilePosition position, history[6];
    u8 pad4c[8];
    struct ProjectilePosition *target;
    u32 unknown58;
    u8 angles[6], radii[6];
    struct ProjectileCollider collider, healthCollider;
    struct ProjectileSprite sprites[6];
    u8 animation[0x1c];
};
struct VanargandProjectilePool {
    u8 pad0[0x18];
    u32 activeMask;
    u16 attack;
};
struct ProjectileProperties {
    s32 first, second;
};
extern const struct ProjectileProperties gUnk_08612114[];
struct VanargandProjectile *VanargandProjectilePool_FindFreeSlot(struct VanargandProjectilePool *,
                                                                 s32 *);
void Sprite_LinkToList(void *, s32);
void AnimData_SelectPaletteBank(void *, u32);
void Collider_SetPosition(void *, void *, u32);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void Collider_SetBodyProperties(void *, s32, s32, s32);
// confidence=low: the extra source position is retained for the approach-launch state.
#define VanargandProjectile_CreateWithSource sub_081B71D4
s32 VanargandProjectile_CreateWithSource(struct VanargandProjectilePool *pool,
                                         struct ProjectilePosition *position,
                                         struct ProjectilePosition *target,
                                         struct ProjectilePosition *source, u32 direction,
                                         u32 speed, u32 type)
{
    s32 index;
    struct VanargandProjectile *projectile = VanargandProjectilePool_FindFreeSlot(pool, &index);
    struct ProjectileSprite *sprite;
    s32 i;
    struct ProjectileCollider *attack;
    struct ProjectileCollider *body;
    struct ProjectilePosition *pos;
    u32 t1;
    if (!projectile)
        return -1;
    {
        u32 zero = 0;
        projectile->state = zero;
        projectile->entered = 1;
        projectile->timer = zero;
        projectile->direction = direction;
        projectile->targetAngle = direction << 8;
        projectile->angle = direction << 8;
        projectile->speed = speed;
        projectile->type = type;
        projectile->steeringTimer = (t1 = 64);
        projectile->trailIndex = 0;
        projectile->target = target;
    }
    projectile->position = *position;
    projectile->unknown58 = (u32)source;
    i = 0;
    attack = &projectile->collider;
    pos = &projectile->position;
    body = &projectile->healthCollider;
    for (; i <= 5; i++) {
        projectile->history[i] = projectile->position;
        sprite = &projectile->sprites[i];
        sprite->position = projectile->position;
        {
            u8 k1 = 1;
            sprite->flags &= ~k1;
        }
        Sprite_LinkToList(sprite, 0);
    }

    AnimData_SelectPaletteBank(projectile->animation, projectile->type + 0x1db);
    Collider_SetPosition(attack, pos, 0);
    Collider_SetPosition(body, pos, 0);
    body->flags |= 4;
    attack->extent = 16;
    body->extent = t1 - 48;
    {
        s32 property = gUnk_08612114[projectile->type].first;
        Collider_SetAttackProperties(attack, pool->attack, 30, 16, property, 30);
    }
    Collider_SetBodyProperties(body, 0, gUnk_08612114[projectile->type].first,
                               gUnk_08612114[projectile->type].second);
    pool->activeMask |= 1 << index;
    return 0;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081B71D4);
#endif
