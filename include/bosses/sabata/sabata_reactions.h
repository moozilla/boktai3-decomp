#ifndef GUARD_SABATA_REACTIONS_H
#define GUARD_SABATA_REACTIONS_H
#include "engine/engine.h"
#include "bosses/sabata/sabata_actor_state.h"
#include "engine/ram.h"

/* Each event sequence step begins with a fresh frame counter. */
static inline void Sabata_EnterSequenceStep(struct SabataActorState *actor, u8 step)
{
    actor->stateReset = step;
    actor->pendingAction = 0;
}

void SceneActors_StartEightDirectionAnimation(void *, void *, void *, int, int, int, int);
void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);
s32 Callback_InvokeActor(void *, void *);
void SceneActors_RunFirstEventScript(void *, void *);
void SceneActors_RunSecondEventScript(void *, void *);
void SceneSteeringActor_RequestRemovalAll(void);
void Sound_PlaySong(u32);
s32 Player_IsSlotInActionTwelveOrThirtyEight(s32);
void Anim_SetFrameDurationScale(void *, u32);
void ActorAnimation_UpdateDirectional(void *);
void ProximityVolume_TranslateBounds(void *, void *);
s32 SabataPosition_TestPath(void *, void *);
s32 Sabata_TestPlayerInAttackArea(void *);
int Math_Atan2(int, int);
u8 *TerrainModifiers_FindCellOverride(u32, int);
void ActorAttachment_PlayEffect(void *, s32);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, s32, s32, s32);
void Sabata_SpawnBobbingFormation(void *);
void ActorEffect_SpawnAtAngle(void *, void *, u32);
void sub_0806CCC8(void *, s32, s32);
void ProximityVolume_SetPosition(void *, void *);
void SabataPosition_SetOffsetPoint(void *);
void SceneActors_SpawnConvergingParticle(void *, void *);
void Sound_StopSong(u32);
void Sabata_ResetAttackSequence(void *);
#endif
