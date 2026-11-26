#ifndef GUARD_RATATOSK_PARTICLE_H
#define GUARD_RATATOSK_PARTICLE_H

#include "engine/video/display_sequence.h"
#include "engine/video/sprite.h"
#include "engine/task.h"

struct RatatoskParticle;
typedef s32 (*RatatoskParticleUpdate)(struct RatatoskParticle *, s32);

struct RatatoskParticleConfig {
    u16 appearanceFlags, lifespan;
    u16 unknown4;
    s8 angle;
    u8 unknown7;
    s32 speed, fallAcceleration, initialFallSpeed;
    RatatoskParticleUpdate callback;
    u32 behavior;
};

struct RatatoskParticle {
    u32 age;
    u8 unknown4[8];
    struct ContactPosition velocity;
    struct RatatoskParticleConfig config;
    struct DisplaySequenceSprite sprite;
    u32 motionFlags;
    s32 fallDistance, fallSpeed;
    struct DisplaySequenceState sequence;
};

struct RatatoskParticlePool {
    struct Task task;
    u32 mask;
    struct SpriteRenderResource graphics;
    struct RatatoskParticle particles[32];
    void *sequenceResource;
    u32 activeCount;
};

typedef char
    RatatoskParticleConfigSizeCheck[sizeof(struct RatatoskParticleConfig) == 0x1C ? 1 : -1];
typedef char RatatoskParticleSizeCheck[sizeof(struct RatatoskParticle) == 0x78 ? 1 : -1];
typedef char RatatoskParticlePoolSizeCheck[sizeof(struct RatatoskParticlePool) == 0xF40 ? 1 : -1];

void Ratatosk_InitAnimatedParticleParams(struct RatatoskParticleConfig *);
void Ratatosk_ConfigureAnimatedParticle(struct RatatoskParticleConfig *, u16 lifespan, u32 speed,
                                        u8 angle, u32 behavior, u32 acceleration, u32 initialSpeed);
void Ratatosk_SetAnimatedParticleUpdateCallback(struct RatatoskParticleConfig *,
                                                RatatoskParticleUpdate);
s32 Ratatosk_OnAnimatedParticleUpdate(struct RatatoskParticle *, s32 slot);
s32 Ratatosk_UpdateParticleFallAndWander(struct RatatoskParticle *, s32 slot);
void OverlaySlot_InitializeExtended(struct RatatoskParticlePool *, struct RatatoskParticle *,
                                    s32 slot);
void Ratatosk_ReleaseAnimatedParticle(struct RatatoskParticlePool *, struct RatatoskParticle *,
                                      u32 slot);
s32 Ratatosk_InitAnimatedParticlePool(struct RatatoskParticlePool *);
struct RatatoskParticlePool *Ratatosk_CreateAnimatedParticlePool(void);
struct RatatoskParticle *Ratatosk_FindFreeAnimatedParticle(struct RatatoskParticlePool *,
                                                           s32 *slot);
u32 Ratatosk_OnAnimatedParticlePoolDestroy(struct RatatoskParticlePool *);
s32 Ratatosk_UpdateAnimatedParticlePool(struct RatatoskParticlePool *);
s32 Ratatosk_SpawnAnimatedParticle(struct RatatoskParticleConfig *, struct ContactPosition *);

#endif
