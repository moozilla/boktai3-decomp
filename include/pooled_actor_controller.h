#ifndef GUARD_POOLED_ACTOR_CONTROLLER_H
#define GUARD_POOLED_ACTOR_CONTROLLER_H

#include "actor_pool.h"
#include "pooled_motion.h"
#include "display_animation_api.h"

struct ActorScriptPosition {
    u32 x : 16, y : 16, z : 16;
};
struct ActorParticlePosition {
    s32 x : 16, y : 16, z : 16;
};

void DamageNumberDisplay_Show(void *, u32, u32, void *, void *, s32);
void TransientSpriteEffects_Spawn(s32, s32, void *, s32, s32, s32);
extern void (*gUnk_0860504C[])(struct PooledActor *, int);
extern void (*const gUnk_08605050[])(void *, int);

void Actor_InitControllerFromScript(struct PooledActor *);
void Actor_PrepareStateUpdate(struct PooledActor *);
void Actor_DispatchActionCallback(struct PooledActor *);
void Actor_UpdateDrawable(struct PooledActor *);
void Actor_UpdateCollisionAndEffectTimers(struct PooledActor *);
void Actor_StartDamageFlash(struct PooledActor *);
int Actor_SetFacingAnimationEntry(struct PooledActor *, int, int, int, int, int);
void Actor_OnColliderContact(void);
void Actor_OnHitboxContact(void *, void *, void *);
s32 Actor_TryStartPendingAction(struct PooledActor *);
int Actor_OnDamage(struct PooledActor *);
void Actor_OnTrackingAction(struct PooledActor *, u32);
void Actor_UpdateAction(struct PooledActor *);
void Actor_UpdateControllerCollisionAndTimers(struct PooledActor *);
void Actor_ShowDamageNumber(struct PooledActor *, u32, u32, void *);
void Actor_OnControllerUpdate(void *);
void Actor_OnControllerDestroy(struct PooledActor *);

#endif
