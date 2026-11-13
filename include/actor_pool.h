#ifndef GUARD_ACTOR_POOL_H
#define GUARD_ACTOR_POOL_H

#include "engine.h"
#include "scene_body_api.h"
#include "scene_facing.h"
#include "player.h"
#include "scene_actor_components.h"
#include "contact_collider.h"

/* Common pooled actor; behavior-specific storage follows this header. */
struct PooledActor {
    struct SceneActorBody body;
    u8 unknown48[4];
    s32 health;
    u32 flags;
    u8 unknown54[4];
    s32 pendingDamage;
    u32 damageSource, attackFlags;
    u8 unknown64[2];
    u16 reaction;
    u8 unknown68[4];
    struct ContactCollider hitSensor, bodyCollider;
    void (*update)(void *);
    void (*destroy)(void *);
    u8 unknown11C[0x1C];
    u32 targetRangeSquared;
    u8 targetArc;
    u8 unknown13D[3];
    u32 targetDistanceSquared;
    u8 targetAngleDelta;
    s8 targetIndex;
    u8 targetCount, updateSlot;
    s32 maxHealth;
    u32 bodyProperty;
    u32 attackProperty0, attackProperty1, attackProperty2, attackProperty3, attackProperty4;
    u32 bodyProperty1, bodyProperty2;
    u32 action;
    u32 actionParameters[4];
    u8 facing4, facing8;
    u8 unknown182[0x96];
    struct PooledActor *next, *previous;
};

struct ActorPoolManager {
    struct Task task;
    struct SceneActorBody *targets[4];
    struct PooledActor *head;
    u32 ticks, updateSlot, serial;
};

struct ActorPoolManager *ActorPool_GetManager(void);
int ActorPool_LinkObject(struct ActorPoolManager *, struct PooledActor *);
int ActorPool_UnlinkObject(struct ActorPoolManager *, struct PooledActor *);
void ActorPool_RefreshTargets(struct ActorPoolManager *);
int ActorPool_UpdateCallback(struct ActorPoolManager *);
int ActorPool_DestroyCallback(struct ActorPoolManager *);
int ActorPool_InitManager(struct ActorPoolManager *);
struct ActorPoolManager *ActorPool_CreateManager(void);
struct PooledActor *ActorPool_AllocObject(int);

void Actor_RequestRemoval(struct PooledActor *);
void Actor_InitEntity(struct PooledActor *, u32, u32, const struct ContactPosition *);
void Actor_SetFacingAngle(struct PooledActor *, int);
void Actor_SetLifecycleCallbacks(struct PooledActor *, void (*)(void *), void (*)(void *));
void Actor_SetHealthLimits(struct PooledActor *, u32, u32);
void Actor_SetHealth(struct PooledActor *, int);
void Actor_SubtractHealth(struct PooledActor *, s32);
void Actor_CapturePendingHit(struct PooledActor *, struct ContactCollider *,
                             struct ContactCollider *);
void Actor_ClearPendingHit(struct PooledActor *);
void Actor_InitHitSensor(struct PooledActor *, void *, void *, void *);
void Actor_InitBodyCollider(struct PooledActor *, void *, void *, void *);
struct SceneActorBody *Actor_GetPartyTarget(u32);
int Actor_FindTargetInFacingArc(struct PooledActor *);
void Actor_RefreshTarget(struct PooledActor *);
void Actor_UpdateSelectedTarget(struct PooledActor *);
void Actor_StepFacingAngle(struct PooledActor *);
void Actor_SetActionParameters(struct PooledActor *, u32, s32, u32 *);
void Collider_SetAttackProperties(void *, u32, u32, u32, u32, u32);
void Collider_SetBodyProperties(void *, u32, u32, u32);

#endif
