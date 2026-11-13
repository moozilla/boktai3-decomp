#ifndef GUARD_FROST_EMITTER_H
#define GUARD_FROST_EMITTER_H

#include "engine.h"
#include "contact_swarm.h"
#include "display_sequence.h"

struct FrostParticle {
    struct ContactSwarmSprite sprite;
    struct DisplaySequenceState sequence;
    struct ContactPosition velocity;
    u16 lifetime, unknown46;
};
struct FrostEmitter;
struct FrostEmitterStatus {
    void (*update)(struct FrostEmitter *);
    u32 age;
};

struct FrostEmitter {
    struct Task task;
    struct ContactSwarmGraphics graphics;
    void *resource;
    struct ContactPosition *source;
    u32 emitting;
    struct FrostParticle particles[12];
    struct FrostEmitterStatus runtime;
};

void SceneNpc_SetSpecialModeFromScript(void);
void FrostEmitter_SpawnParticle(struct FrostEmitter *);
void FrostEmitter_UpdateParticle(struct FrostParticle *);
void FrostEmitter_Step(struct FrostEmitter *);
void FrostEmitter_Update(struct FrostEmitter *);
void FrostEmitter_Destroy(struct FrostEmitter *);
s32 FrostEmitter_Initialize(struct FrostEmitter *);
struct FrostEmitter *FrostEmitter_CreateTask(void);
struct FrostEmitter *FrostEmitter_Create(void);
void FrostEmitter_StartEmission(struct FrostEmitter *, struct ContactPosition *);
void FrostEmitter_StopEmission(struct FrostEmitter *);
#endif
