#ifndef GUARD_SCENE_ACTOR_ACTIONS_H
#define GUARD_SCENE_ACTOR_ACTIONS_H

#include "scene_actor_lifecycle.h"

int Actor_UpdateMovementAnimationAndEffects(void *unused, struct SceneActor *p);
void Actor_NoopUpdate(void);
void Actor_NoopRender(void);
int Actor_UpdateVisibilityAndShadow(void *unused, struct SceneActor *p);
int Actor_SetMirroredFourWayAnimation(struct SceneActor *p, int a, int b, int stride, int c, int d);
int Actor_SetFourWayAnimation(struct SceneActor *p, int a, int b, int c, int d, int e);
int Actor_SetMirroredEightWayAnimation(struct SceneActor *p, int a, int b, int stride, int c,
                                       int d);
int Actor_SetFrontBackAnimation(struct SceneActor *p, int a, int b, int stride, int c, int d);
u32 Actor_SetUndirectedAnimation(struct SceneActor *p, u32 x, u32 y, u32 z, u32 s0, u32 s1);
int Actor_PlayAnimationEntry(void *actor, int id, int a, int b, int c);
void Actor_PlayFootstepSound(struct SceneActor *actor, u32 mode);
u32 Actor_IsWithinHorizontalRange(struct ContactPosition *a, struct ContactPosition *b, s32 range);
void Actor_SetAttachmentEffect(struct SceneActor *p, unsigned kind, int timer, int duration);
void Actor_EmitTowardNearbyActors(struct SceneActorBody *emitter);
void Actor_UpdateInteractionCue(struct SceneActorManager *input);
s32 Packet_QueueFromScript(void);
int Actor_BeginScriptControl(void);
int Actor_EndScriptControl(void);
u32 Actor_IsInReactionStateFromScript(void);
int Actor_InitBehaviorFromScript(void *unused, struct SceneActor *p);
void Actor_OnHiddenIdleUpdate(void *unused, struct SceneActor *p);
void Actor_OnIdleUpdate(void *unused, struct SceneActor *p);
void Actor_UpdateWalkToPosition(void *unused, struct SceneActor *arg);
void Actor_UpdateTurnToAngle(void *unused, struct SceneActor *p);
void Actor_OnDescendingFadeUpdate(void *unused, struct SceneActor *p, int ticks);
void Actor_OnRisingFadeUpdate(void *unused, struct SceneActor *p, int ticks);
void Actor_OnConfiguredAnimationUpdate(void *unused, struct SceneActor *p);
void Actor_OnPathFollowUpdate(void *unused, struct SceneActor *p);
void Actor_UpdateTargetFacing(struct SceneActorManager *ctx, struct SceneActor *p);
void Actor_OnTimedIdleUpdate(void *unused, struct SceneActor *p, int ticks);
void Actor_UpdateNavigationWalk(void *unused, struct SceneActor *p);
void Actor_UpdateNavigationStep(void *unused, struct SceneActor *p);
void SceneActorAnimation_UpdateRouteWalk(void *unused, struct SceneActor *p);
void Actor_UpdateRunToPosition(void *unused, struct SceneActor *arg);
int Actor_SelectPathNodeAction(struct SceneActor *p, struct SceneActorRoutePoint *node);
int SceneCharacter_GetPathNodeState(struct SceneActor *p);
void SceneCharacter_UpdateIdleState(u32 a, struct SceneActor *p);
void SceneCharacter_UpdateStandState(u32 a, struct SceneActor *p);
void SceneCharacter_UpdateOneShotActionState(void *unused, struct SceneActor *p);
void SceneCharacter_UpdateTurnState(void *unused, struct SceneActor *p);
void SceneCharacter_UpdateAlternateOneShotActionState(void *unused, struct SceneActor *p);
void SceneCharacter_UpdateStopState(void *unused, struct SceneActor *p);
void SceneCharacter_UpdateProximityWaitState(struct SceneActorManager *m, struct SceneActor *p);
void SceneCharacter_UpdatePathWaitState(struct SceneActorManager *m, struct SceneActor *p);
void SceneCharacter_UpdatePathTurnState(struct SceneActorManager *m, struct SceneActor *p);
void SceneCharacter_UpdatePathActionState(struct SceneActorManager *m, struct SceneActor *p);
void SceneCharacter_UpdateTargetProximityState(struct SceneActorManager *m, struct SceneActor *p);
void SceneCharacter_UpdateTetherReactionState(u8 *a, struct SceneActor *p);
void SceneCharacter_UpdatePhaseReactionState(u8 *a, struct SceneActor *p);
void SceneCharacter_UpdatePhaseTransitionState(struct SceneActorManager *m, struct SceneActor *p);
void SceneCharacter_UpdatePhaseWindowWaitState(struct SceneActorManager *m, struct SceneActor *p);
void SceneCharacter_UpdatePathResumeState(struct SceneActorManager *m, struct SceneActor *p);

#endif
