#ifndef GUARD_PARTICLE_POOL_H
#define GUARD_PARTICLE_POOL_H
#include "engine.h"
#include "scene_vec.h"
#include "engine_globals.h"
#include "monster_particle_random.h"
union ParticlePoolPosition {
    struct SceneVec signedAxis;
    struct {
        u16 x, y, z, padding;
    } axis;
    u32 words[2];
};
struct ParticlePoolControl {
    u8 active, angle, riseSpeed, expandDelay;
    u8 animationPhase, angularSpeed, lifetime, state;
    u16 radius;
    u8 kind, unknown33;
    s16 velocityX, velocityY, velocityZ, unknown3A;
};
struct PoolParticleTail {
    union ParticlePoolPosition position;
    u8 unknown20[8];
    struct ParticlePoolControl control;
};
struct PoolParticle {
    u32 flags;
    u8 unknown4[11], renderMode, priority, unknown11[7];
    struct PoolParticleTail tail;
};
struct ParticlePool {
    u8 task[0x18];
    u8 collider[0x58];
    void *resource;
    struct PoolParticle particles[16];
    u16 frame;
    u8 unknown436[6];
};
struct ParticlePoolVolumeVector {
    u32 x : 16, y : 16, z : 16;
};
void SpriteDrawable_SelectTileFrame(void *, void *, u32);
void Palette_UpdateHighlightPulse(u16 *, u8 *);
void SpiralParticles_UpdateParticle(struct ParticlePool *, struct PoolParticle *);
void ParticlePool_UpdateLinearParticle(struct ParticlePool *, struct PoolParticle *);
void ParticlePool_SpawnJitteredParticle(struct ParticlePool *, struct SceneVec *, s32, u32);
void SpriteParticles_UpdateEntries(struct ParticlePool *);
s32 ParticlePool_OnUpdate(struct ParticlePool *);
s32 ParticlePool_OnDestroy(struct ParticlePool *);
void CollisionVolume_InitCollider(struct ParticlePool *);
void ParticlePool_InitParticles(struct ParticlePool *);
s32 ParticlePool_Init(struct ParticlePool *);
struct ParticlePool *ParticlePool_Create(void);
void ParticlePool_Destroy(struct ParticlePool *);
void ParticlePool_EmitTriple(struct ParticlePool *, struct SceneVec *, s32, u32);
#endif
