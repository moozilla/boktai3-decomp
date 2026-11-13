#ifndef GUARD_EXPLOSION_H
#define GUARD_EXPLOSION_H
#include "engine.h"
#include "contact_swarm.h"
struct Explosion {
    struct Task task;
    struct ContactSwarmSprite sprite;
    struct ContactSwarmGraphics graphics;
    struct ContactCollider collider;
    u16 delay, age;
    s16 collisionDisabled, silent;
};
struct ExplosionSize {
    u32 x : 16, y : 16, z : 16;
};
s32 Explosion_Update(struct Explosion *explosion);
u32 Explosion_Destroy(struct Explosion *explosion);
void Explosion_InitSprite(struct ContactSwarmSprite *sprite, struct ContactSwarmGraphics *resource,
                          struct ContactPosition *position, s32 palette);
void Explosion_InitCollider(struct Explosion *explosion, struct ContactCollider *collider,
                            u32 contactFlags, s32 power, s32 force, s32 reactionFlags,
                            s32 hitDuration);
void Explosion_InitColliderWithFlags(struct Explosion *explosion, struct ContactCollider *collider,
                                     u32 contactFlags, s32 collisionProperties, s32 power,
                                     s32 force, s32 reactionFlags, s32 hitDuration);
s32 Explosion_Init(struct Explosion *explosion, struct ContactPosition *position, u32 contactFlags,
                   s32 power, s32 force, s32 reactionFlags, s32 hitDuration, s32 palette,
                   u32 delay);
s32 Explosion_InitWithCollisionFlags(struct Explosion *explosion, struct ContactPosition *position,
                                     u32 contactFlags, s32 collisionProperties, s32 power,
                                     s32 force, s32 reactionFlags, s32 hitDuration, s32 palette,
                                     u32 delay);
struct Explosion *Explosion_CreateImmediate(struct ContactPosition *position, u32 contactFlags,
                                            s32 power, s32 force, s32 palette);
struct Explosion *Explosion_CreateDelayed(struct ContactPosition *position, u32 contactFlags,
                                          s32 power, s32 force, s32 palette, u32 delay);
struct Explosion *Explosion_Create(struct ContactPosition *position, u32 contactFlags, s32 power,
                                   s32 force, s32 reactionFlags, s32 hitDuration, s32 palette,
                                   u32 delay);
struct Explosion *Explosion_CreateAt(struct ContactPosition *position, u32 contactFlags, s32 power,
                                     s32 force, s32 reactionFlags, s32 hitDuration, s32 palette,
                                     u32 delay);
struct Explosion *Explosion_CreateWithCollisionFlags(struct ContactPosition *position,
                                                     u32 contactFlags, s32 collisionProperties,
                                                     s32 power, s32 force, s32 reactionFlags,
                                                     s32 hitDuration, s32 palette, u32 delay);
#endif
