#ifndef GUARD_SCENE_EVENT_ACTOR_H
#define GUARD_SCENE_EVENT_ACTOR_H
#include "engine.h"
#include "actor_state.h"
#include "ram.h"
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
s32 ActorPosition_TestPath(void *, void *);
s32 SceneVolume_TestPlayerPosition(void *);
int Math_Atan2(int, int);
u8 *TerrainModifiers_FindCellOverride(u32, int);
void ActorAttachment_PlayEffect(void *, s32);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, s32, s32, s32);
void BossSlot2_SpawnBobbingFormation(void *);
void ActorEffect_SpawnAtAngle(void *, void *, u32);
void sub_0806CCC8(void *, s32, s32);
void ProximityVolume_SetPosition(void *, void *);
void ActorPosition_SetOffsetPoint(void *);
void SceneActors_SpawnConvergingParticle(void *, void *);
void Sound_StopSong(u32);
void SceneVolume_ResetPendingSequence(void *);
#endif
