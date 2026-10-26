#ifndef GUARD_SPARK_PARTICLE_H
#define GUARD_SPARK_PARTICLE_H
#include "contact_swarm.h"
/* Tentative identity: forty-eight independent moving tile-animation particles. */
struct SparkParticleResource {
    u32 unknown0;
    u16 tileBase;
};
struct SparkParticleSprite {
    u32 flags;
    u8 unknown4[4];
    u16 tile;
    u8 width, height;
    u8 unknownC[3], priority, renderMode;
    u8 unknown11[7];
    struct ContactPosition position;
    u8 unknown20[8];
};
/* Narrow view of the velocity/active tail at entry offset38. */
struct SparkParticleMotion {
    struct ContactPosition velocity;
    u8 active, unknown41[3];
};
struct SparkParticle {
    struct DisplaySequenceState sequence;
    struct SparkParticleSprite sprite;
    struct ContactPosition velocity;
    u8 active, unknown41[3];
};
struct SparkParticlePool {
    u8 task[0x18];
    struct SparkParticle particles[48];
    const void *sequenceResource;
    const struct SparkParticleResource *graphics;
    u32 activeCount;
};
extern struct SparkParticlePool *gSparkParticlePool;
s32 SparkParticle_UpdatePool(struct SparkParticlePool *);
s32 SparkParticle_DestroyPool(struct SparkParticlePool *);
s32 SparkParticle_Spawn(const struct ContactPosition *, u8, const struct ContactPosition *, s32,
                        s32, s32, s32, u32);
void sub_0806A420(void);
s32 SparkParticle_InitPool(struct SparkParticlePool *);
struct SparkParticlePool *SparkParticle_CreatePool(void);
void SparkParticle_ClearPool(void);
void ParticleSprite_Initialize(void *, const void *, u32);
void Effect_SetOrigin(void *, s32, s32);
void Effect_SetPalette(void *, u32);
#endif
