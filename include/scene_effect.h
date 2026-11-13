#ifndef GUARD_SCENE_EFFECT_H
#define GUARD_SCENE_EFFECT_H
#include "engine.h"
#include "projected_display.h"
#include "contact_swarm_sprite.h"
#include "display_sequence.h"
struct SceneEffectQueue;
/* One shadowed actor and four traveling rings belong to each queue. */
struct SceneEffectActor {
    void (*update)(void *), (*destroy)(void *);
    u8 animationData[0x20];
    struct ProjectedDisplayEntry animation;
    u8 shadow[0x6C];
    struct ContactCollider collider;
    struct ContactPosition velocity;
    struct SceneEffectQueue *owner;
    u8 entered, phase, animationEnded, state;
};
struct SceneEffectRing {
    void (*update)(void *), (*destroy)(void *);
    u8 animation[0x1C];
    struct ContactSwarmSprite sprite;
    struct DisplaySequenceState sequence;
    struct ContactCollider collider;
    u8 mode, animationEnded;
};
struct SceneEffectRequest {
    u8 type, side, remaining, unknown3;
};
struct SceneEffectQueue {
    struct Task task;
    u32 messageId;
    u8 actorsUsed, ringsUsed, writeIndex, readIndex;
    u16 actorPower, actorElement, ringPower, ringElement, specialPower, specialElement;
    struct SceneEffectRequest requests[2];
    struct SceneEffectActor actors[1];
    struct SceneEffectRing rings[4];
};
struct SceneEffectPackedPosition {
    u32 x : 16, y : 16, z : 16, unused : 16;
};
void ShadowActor_UpdateTransition(struct SceneEffectActor *);
void SceneEffect_UpdateFinishAnimation(struct SceneEffectActor *);
void SceneActor_SetSavedPositionOffset(struct SceneEffectActor *);
void ShadowActor_UpdateThreeStageAction(struct SceneEffectActor *);
void SceneEffect_SetScreenEdgePosition(struct SceneEffectActor *);
void SceneEffect_UpdateRingSpawn(struct SceneEffectActor *);
void SceneEffect_UpdateArrival(struct SceneEffectActor *);
void SceneEffect_UpdateRecoil(struct SceneEffectActor *);
void SceneEffect_UpdateActor(void *);
void SceneEffect_DestroyActor(void *);
void SceneEffect_InitActor(struct SceneEffectActor *, s32, s32);
struct SceneEffectActor *SceneEffect_AllocateActor(struct SceneEffectQueue *, s32, s32);
u32 SceneEffect_LoadRingAnimation(void);
void SceneEffectRing_Update(void *);
void SceneEffect_DestroyRing(void *);
void SceneEffect_InitializeRing(struct SceneEffectQueue *, struct SceneEffectRing *,
                                struct ContactPosition *, u32);
void SceneEntryPool_CreateEntry(struct SceneEffectQueue *, struct ContactPosition *, u32);
#endif
