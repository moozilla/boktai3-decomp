#include "global.h"

// Exact scope match in plain C.
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
s32 VanargandProjectile_Create(struct VanargandProjectilePool *pool,
                               struct ProjectilePosition *position,
                               struct ProjectilePosition *target, u32 direction, u32 speed,
                               u32 type)
{
    s32 index;
    struct VanargandProjectile *projectile = VanargandProjectilePool_FindFreeSlot(pool, &index);
    struct ProjectileSprite *sprite;
    s32 i;
    struct ProjectileCollider *attack, *body;
    struct ProjectilePosition *pos;
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
        projectile->steeringTimer = 64;
        projectile->trailIndex = 0;
        projectile->target = target;
        projectile->unknown58 = zero;
    }
    projectile->position = *position;
    sprite = projectile->sprites;
    i = 0;
    attack = &projectile->collider;
    pos = &projectile->position;
    body = &projectile->healthCollider;
initializeSprite:
    *(i + projectile->history) = projectile->position;
    sprite->position = projectile->position;
    sprite->flags &= ~1;
    Sprite_LinkToList(sprite, 0);
    i++;
    sprite++;
    if (i <= 5)
        goto initializeSprite;
    {
        void *animation = projectile->animation;
        u32 palette = projectile->type + 0x1db;
        AnimData_SelectPaletteBank(animation, (u16)palette);
    }
    Collider_SetPosition(attack, pos, 0);
    Collider_SetPosition(body, pos, 0);
    body->flags |= 4;
    attack->extent = 16;
    body->extent = 16;
    {
        s32 property = gUnk_08612114[projectile->type].first;
        Collider_SetAttackProperties(attack, pool->attack, 30, 16, property, 30);
    }
    Collider_SetBodyProperties(body, 0, gUnk_08612114[projectile->type].first,
                               gUnk_08612114[projectile->type].second);
    pool->activeMask |= 1 << index;
    return 0;
}
