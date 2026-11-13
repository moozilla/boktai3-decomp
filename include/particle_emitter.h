#ifndef GUARD_PARTICLE_EMITTER_H
#define GUARD_PARTICLE_EMITTER_H

#include "engine.h"
#include "player_cue_sequence.h"
#include "scene_pool.h"
#include "scene_arc.h"

/* The first two helpers belong to the preceding four-slot display manager. */
struct ParticleEmitterDisplaySlot {
    u8 resource[0x1c];
    struct ContactSwarmSprite sprite;
    struct DisplaySequenceState animation;
    void *source;
    u16 stopRequested, active;
};
struct ParticleEmitterDisplay {
    u8 task[0x18];
    struct ParticleEmitterDisplaySlot slots[4];
    void *sequenceResource;
};

/* Tentative flying emitter: fixed-point world coordinates, then a scatter trail. */
struct ParticleEmitter {
    void (*update)(struct ParticleEmitter *);
    u8 unknown04[0x1c];
    struct ContactSwarmSprite sprite;         /* 0x20 */
    struct DisplaySequenceState animation;    /* 0x4c */
    u8 spatialEntry[0x40];                    /* 0x5c: spatial-index collision record */
    struct ScenePoolVec32 position, velocity; /* 0x9c, 0xac */
    u8 unknownBC[0x10];
    s32 heightOffset, initialVelocity; /* 0xcc, 0xd0 */
    u8 entered, active, phase, unknownD7;
    u16 timer;
    u8 owner;
};

void TimedSpriteEffect_RequestStop(u32 slot);
void ParticleEmitter_ResetDisplay(void);
u8 ParticleEmitter_IsBelowHeightLimit(struct ParticleEmitter *);
void ParticleEmitter_OnStop(struct ParticleEmitter *);
void ParticleEmitter_UpdateScatterTrail(struct ParticleEmitter *);
void ParticleEmitter_UpdateFlight(struct ParticleEmitter *);

#endif
