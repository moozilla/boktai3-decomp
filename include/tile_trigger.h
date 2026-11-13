#ifndef GUARD_TILE_TRIGGER_H
#define GUARD_TILE_TRIGGER_H
#include "player_cue_sequence.h"
#include "scene_vec.h"
#include "scene_actor_components.h"
#include "scene_animation.h"
struct TileTrigger {
    struct ContactSwarmSprite sprite;
    struct SceneActorBody body;
    struct DisplaySequenceState animation;
    struct SceneVec position;
    s32 key;
    u32 pendingScript;
    u16 oneShotScript, unknown96;
    s32 arguments[6], oneShotArguments[2];
    u16 sequence, delay;
    s16 index, state;
    u16 triggerFlags;
    u8 forced, frame;
    s32 timer;
};
struct TileTriggerManager {
    struct Task task;
    void *animationData;
    struct SceneAnimation graphics;
    struct TileTrigger *tiles[12];
    u32 used;
};
struct TileTriggerScriptArgs {
    u32 count : 16, unused : 16;
    s32 *values;
};
void ScriptTriggerActor_DispatchPendingScripts(struct TileTrigger *);
void TileTrigger_ApplyContactEffects(struct TileTrigger *);
u8 TileTrigger_IsTetherNearby(struct TileTrigger *);
void TileTrigger_EmitParticles(struct TileTrigger *);
void TileTrigger_RestartHold(struct TileTriggerManager *, struct TileTrigger *);
void Pool1B8_EnterStateThree(struct TileTriggerManager *, struct TileTrigger *);
struct TileTriggerManager *Pool1B8_Create(void);
struct TileTrigger *Pool1B8_Allocate(struct TileTriggerManager *);
struct TileTrigger *Pool1B8_FindByName(u32);
s32 Pool1B8_Init(struct TileTriggerManager *);
s32 TileTrigger_TickManager(struct TileTriggerManager *);
u32 TilePool_Destroy(struct TileTriggerManager *);
void SceneBody_Init(void *, s32, void *, u32, u32, void *);
void SceneBody_SetSprite(void *, void *);
void TerrainOverlay_QueueAddActor(void *);
u32 TerrainOverlay_QueueRemoveActor(void *);
void SceneBody_Unregister(void *);
#endif
