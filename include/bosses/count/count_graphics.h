#ifndef GUARD_BOSSES_COUNT_COUNT_GRAPHICS_H
#define GUARD_BOSSES_COUNT_COUNT_GRAPHICS_H
#include "engine/engine.h"
#include "npc/story_character/state_actor.h"
#include "scene/collision/contact_actor.h"
#include "effects/screen/background_flash.h"

#include "bosses/count/scene_graphics_actor.h"

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
