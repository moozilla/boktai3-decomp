#ifndef GUARD_FROST_EMITTER_H
#define GUARD_FROST_EMITTER_H

#include "engine/engine.h"
#include "scene/collision/contact_swarm.h"
#include "engine/video/display_sequence.h"

struct FrostParticle {
    struct ContactSwarmSprite sprite;
    struct DisplaySequenceState sequence;
    struct ContactPosition velocity;
    u16 lifetime, unknown46;
};
struct SabataFrost;
struct SabataFrostStatus {
    void (*update)(struct SabataFrost *);
    u32 age;
};

struct SabataFrost {
    struct Task task;
    struct ContactSwarmGraphics graphics;
    void *resource;
    struct ContactPosition *source;
    u32 emitting;
    struct FrostParticle particles[12];
    struct SabataFrostStatus runtime;
};

void SceneNpc_SetSpecialModeFromScript(void);
void SabataFrost_SpawnParticle(struct SabataFrost *);
void SabataFrost_UpdateParticle(struct FrostParticle *);
void SabataFrost_Step(struct SabataFrost *);
void SabataFrost_Update(struct SabataFrost *);
void SabataFrost_Destroy(struct SabataFrost *);
s32 SabataFrost_Initialize(struct SabataFrost *);
struct SabataFrost *SabataFrost_CreateTask(void);
struct SabataFrost *SabataFrost_Create(void);
void SabataFrost_StartEmission(struct SabataFrost *, struct ContactPosition *);
void SabataFrost_StopEmission(struct SabataFrost *);
#endif
