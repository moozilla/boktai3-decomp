#ifndef GUARD_SCENE_PROJECTILE_POOL_H
#define GUARD_SCENE_PROJECTILE_POOL_H
#include "engine.h"
#include "contact_swarm.h"
#include "sun_orb_effect.h"
#include "scene_projectile_params.h"
struct SceneParticleLaunchParams {
    u16 flags, lifetime, unknown4;
    u8 direction, unknown7;
    u32 speed, scale;
    void *callback;
    u32 kind;
};
struct SceneProjectileLaunch {
    s32 speed;
    u8 mode, rotation;
    u16 unknown6;
    s16 contactCount;
    s16 scale;
};
struct SceneProjectile {
    u32 age, flags;
    u8 unknown8[8];
    struct ContactPosition velocity;
    struct DisplaySequenceState sequence;
    struct SceneProjectileParams params;
    struct ContactSwarmGraphics graphics;
    struct OrbitEffectSprite sprite;
    struct ContactCollider attack, body;
    struct SceneProjectileLaunch launch;
    u8 unknown17C[0x44];
};
struct SceneProjectilePool {
    struct Task task;
    u32 used, hitEvent;
    struct SceneProjectile slots[10];
};
struct SceneProjectileColliderVector {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
u32 SceneProjectile_ConsumeHitEvent(void);
void SceneProjectile_UpdateSpriteRotation(struct SceneProjectile *p);
s32 SceneProjectile_UpdatePulse(struct SceneProjectile *p);
s32 SceneProjectile_EmitPeriodicParticle(struct SceneProjectile *p);
u32 SceneProjectile_UpdateHomingRotation(struct SceneProjectile *p);
void SceneProjectile_RespondToContact(void *unused, struct ContactCollider *contact,
                                      struct SceneProjectile *p);
void SlotActor_ResetOnFlags(struct ContactCollider *signal, struct ContactCollider *state,
                            struct SceneProjectile *actor);
void SceneProjectile_InitSlot(void *pool, struct SceneProjectile *p, s32 index);
void SceneProjectile_ReleaseSlot(struct SceneProjectilePool *a, struct SceneProjectile *b, u32 n);
s32 SceneProjectile_UpdatePool(struct SceneProjectilePool *pool);
u32 SceneProjectile_OnPoolDestroy(struct SceneProjectilePool *a);
s32 SceneProjectile_InitPool(struct SceneProjectilePool *a);
void *SceneProjectile_CreatePool(void);
#endif
