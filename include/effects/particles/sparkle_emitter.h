#ifndef GUARD_SPARKLE_EMITTER_H
#define GUARD_SPARKLE_EMITTER_H
#include "engine/engine.h"
#include "effects/projectiles/contact_swarm_sprite.h"
#include "scene/actor/scene_event_dispatch.h"
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
void CutsceneSparkle_UpdateParticles(struct SparkleEmitter *);
struct SparkleParticle *ParticlePool_FindFreeEntry(struct SparkleEmitter *);
void CutsceneSparkle_EmitParticle(struct SparkleEmitter *);
void CutsceneSparkle_OnUpdateHook(void *);
void CutsceneSparkle_UpdateEmitting(struct SparkleEmitter *);
void CutsceneSparkle_UpdateDraining(struct SparkleEmitter *);
void CutsceneSparkle_ProcessMessages(struct SparkleEmitter *);
void CutsceneSparkle_OnTaskUpdate(void *);
void CutsceneSparkle_OnTaskDestroy(void *);
s32 CutsceneSparkle_Initialize(struct SparkleEmitter *, void *);
void CutsceneSparkle_ReconfigureFromScript(void);
void CutsceneSparkle_StopEmitting(void);
void *CutsceneSparkle_CreateFromScript(void *);
#endif
