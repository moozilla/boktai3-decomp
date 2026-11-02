#ifndef GUARD_PROJECTILE_POOL_H
#define GUARD_PROJECTILE_POOL_H

#include "display_animation.h"
#include "contact_collider.h"

struct ProjectileSlot;
typedef s32 (*ProjectileSlotUpdate)(struct ProjectileSlot *slot, s32 index);

/* The spawn routine copies this 0x5c-byte configuration into the live slot. */
struct ProjectileConfig {
    u16 movement, duration, homingFrames;
    s8 angle, turnStep;
    s32 speed;
    u32 bodyId;
    ProjectileSlotUpdate callback;
    u32 rewardFlags, attackFlags;
    s16 power, knockback, variants, immunity;
    struct ContactPosition bounds, offset;
    u32 graphicsId, frame;
    s32 palette;
    s32 gravity, velocity, height;
    u16 baseY;
    s16 spriteFlag;
    u32 script, sequence;
    s16 persist;
    u8 unknown5A[2];
};

struct ProjectileSlot {
    u32 timer, flags;
    u8 unknown8[8];
    struct ContactPosition velocity;
    struct DisplaySequenceState anim;
    struct ProjectileConfig params;
    u8 graphics[0x1c];
    struct DisplaySequenceSprite sprite;
    struct ContactCollider collider;
    struct ContactPosition *target;
    void *scriptProc;
};

struct TimedProjectilePool {
    u8 task[0x18];
    u32 mask;
    struct ProjectileSlot slots[16];
};

s32 Projectile_SpawnFromConfig(struct ProjectileConfig *params, struct ContactPosition *pos);
void ProjectilePool_ReleaseSlot(struct TimedProjectilePool *pool, struct ProjectileSlot *slot,
                                u32 index);
struct ProjectileSlot *ProjectilePool_FindFreeSlot(struct TimedProjectilePool *pool, s32 *index);

void TimedSpriteEffect_InitSlot(struct TimedProjectilePool *pool, struct ProjectileSlot *slot,
                                s32 index);

#endif
