#ifndef GUARD_ACTOR_GRAPHICS_AND_BASIC_STATES_H
#define GUARD_ACTOR_GRAPHICS_AND_BASIC_STATES_H
#include "engine/shared/engine.h"
#include "engine/shared/state_actor.h"
#include "engine/shared/contact_actor.h"
#include "effects/bg1_flash_and_scrolling_controller/background_flash.h"

#include "engine/shared/scene_graphics_actor.h"

void SceneActors_StartDirectionalAnimation(void *, void *, void *, s32, s32, s32, s32);
void SceneActors_StartEightDirectionAnimation(void *, void *, void *, s32, s32, s32, s32);
struct StateActor *SceneActors_GetBySlot(s32);
void SceneActors_ConfigureEffect(struct StateActor *, s32, s32, s32);
void SceneMessages_SetPendingTrigger(void *entry, void *message);
void BackgroundFlash_BeginAppearance(struct BackgroundFlashController *);
void BackgroundFlash_BeginAlternating(struct BackgroundFlashController *);
void BackgroundFlash_BeginDisappearance(struct BackgroundFlashController *);
s32 AreaEffect_IsPositionCovered(void *position);

#endif
