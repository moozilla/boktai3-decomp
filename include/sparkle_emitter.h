#ifndef GUARD_SPARKLE_EMITTER_H
#define GUARD_SPARKLE_EMITTER_H
#include "engine.h"
#include "contact_swarm_sprite.h"
#include "scene_event_dispatch.h"
struct SparkleParticle {
    struct ContactSwarmSprite sprite;
    struct ContactPosition velocity;
    s32 age;
};
struct SparkleEmitter {
    struct Task task;
    struct SparkleParticle particles[32];
    u8 animation[0x1C];
    struct SceneEventReceiver receiver;
    u8 receiverState[0xC];
    s32 palette;
    void *script;
    s32 timer;
    s16 angle, speed;
    s32 count;
    void (*update)(struct SparkleEmitter *);
};
struct SparkleEmitterCommand {
    struct SceneEventCommand command;
    s16 palette;
};
void SparkleEmitter_UpdateParticles(struct SparkleEmitter *);
struct SparkleParticle *ParticlePool_FindFreeEntry(struct SparkleEmitter *);
void SparkleEmitter_EmitParticle(struct SparkleEmitter *);
void SparkleEmitter_OnUpdateHook(void *);
void SparkleEmitter_UpdateEmitting(struct SparkleEmitter *);
void SparkleEmitter_UpdateDraining(struct SparkleEmitter *);
void SparkleEmitter_ProcessMessages(struct SparkleEmitter *);
void SparkleEmitter_OnTaskUpdate(void *);
void SparkleEmitter_OnTaskDestroy(void *);
s32 SparkleEmitter_Initialize(struct SparkleEmitter *, void *);
void SparkleEmitter_ReconfigureFromScript(void);
void SparkleEmitter_StopEmitting(void);
void *SparkleEmitter_CreateFromScript(void *);
#endif
