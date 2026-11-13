#ifndef GUARD_VANARGAND_PROJECTILES_H
#define GUARD_VANARGAND_PROJECTILES_H

#include "engine.h"
#include "contact_swarm.h"
#include "transient_sprite_pool.h"
#include "monster_particle_random.h"

struct VanargandProjectile {
    u8 entered, state, type;
    u8 unknown3[3];
    u16 timer;
    u8 trailIndex, direction;
    u16 targetAngle, angle, steeringTimer, speed;
    u8 unknown12[2];
    struct ContactPosition position, history[6], origin;
    struct ContactPosition *target, *targetSource;
    u8 angles[6], radii[6];
    struct ContactCollider collider, healthCollider;
    struct ContactSwarmSprite sprites[6];
    struct TransientSpriteGraphics graphics;
};

struct VanargandProjectilePool {
    u8 task[0x18];
    u32 activeMask;
    u16 attack;
    u8 unknown1E[6];
    struct VanargandProjectile slots[8];
};

struct VanargandAfterimage {
    s32 lifetime;
    struct ContactSwarmSprite sprite;
    struct TransientSpriteGraphics graphics;
};

struct VanargandAfterimagePool {
    u8 task[0x18];
    s32 count;
    struct VanargandAfterimage slots[8];
};

struct VanargandProjectileProperties {
    s32 reaction, damageScale; /* Tentative collider property meanings. */
};

struct VanargandProjectile *VanargandProjectilePool_FindFreeSlot(struct VanargandProjectilePool *,
                                                                 s32 *);
s32 VanargandProjectile_Initialize(struct VanargandProjectilePool *, struct VanargandProjectile *,
                                   s32);
void VanargandProjectile_OnContact(struct ContactCollider *, void *, struct VanargandProjectile *);
void VanargandProjectile_IgnoreContact(void);
u32 VanargandProjectile_Release(struct VanargandProjectilePool *, struct VanargandProjectile *,
                                u32);
void VanargandProjectile_UpdatePositionTrail(struct VanargandProjectile *);
void VanargandProjectile_UpdateRadialTrail(struct VanargandProjectile *);
void VanargandProjectile_TurnTowardTarget(struct VanargandProjectile *);
s32 VanargandProjectilePool_Update(struct VanargandProjectilePool *);
u32 VanargandProjectilePool_Destroy(struct VanargandProjectilePool *);
s32 VanargandProjectilePool_Init(struct VanargandProjectilePool *, s32);
struct VanargandAfterimage *VanargandAfterimagePool_FindFreeSlot(struct VanargandAfterimagePool *);
void VanargandAfterimagePool_ActivateSlot(struct VanargandAfterimage *, struct ContactPosition *);
void VanargandAfterimagePool_InitializeSlot(void *, struct VanargandAfterimage *, s32);
void VanargandAfterimagePool_ReleaseSlot(struct VanargandAfterimagePool *,
                                         struct VanargandAfterimage *);
void VanargandAfterimagePool_UpdateSlot(struct VanargandAfterimagePool *,
                                        struct VanargandAfterimage *, u32);
void VanargandAfterimagePool_Update(struct VanargandAfterimagePool *);
void VanargandAfterimagePool_Destroy(struct VanargandAfterimagePool *);
s32 VanargandAfterimagePool_Init(struct VanargandAfterimagePool *);

void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void Collider_SetBodyProperties(void *, s32, s32, s32);
void Sprite_InitUnlinked(void *, void *, s32);
void Sprite_LinkToList(void *, s32);
void PlayerStats_AddContactHundredths(void *, void *, u32);
void Collision_CalculateDamageResponse(void *);
struct ContactPosition *SceneActors_GetAvailablePosition(void);
void Vec16_Lerp(void *, void *, void *, int, int);

static inline s32 VanargandProjectile_Scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -(-value >> 12);
}

static inline void MakeOffset(struct ContactPosition *v, u32 angle, s32 radius)
{
    v->x = VanargandProjectile_Scale(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = VanargandProjectile_Scale(gUnk_086149C4[angle & 255] * radius);
}

static inline void EffectCollision_Disable(struct ContactCollider *collider)
{
    collider->flags |= 4;
}

typedef char VanargandProjectileSizeCheck[sizeof(struct VanargandProjectile) == 0x234 ? 1 : -1];
typedef char
    VanargandProjectilePoolSizeCheck[sizeof(struct VanargandProjectilePool) == 0x11C4 ? 1 : -1];
typedef char VanargandAfterimageSizeCheck[sizeof(struct VanargandAfterimage) == 0x4C ? 1 : -1];
typedef char
    VanargandAfterimagePoolSizeCheck[sizeof(struct VanargandAfterimagePool) == 0x27C ? 1 : -1];

#endif
