#ifndef GUARD_SHIPWHEEL_PRESENTATION_H
#define GUARD_SHIPWHEEL_PRESENTATION_H
#include "engine.h"
#include "contact_swarm.h"
#include "scene_event_dispatch.h"
#include "projected_display.h"
#include "contact_impact_effect.h"
struct WheelPaletteTransition {
    void (*update)(struct WheelPaletteTransition *);
    u32 timer;
    const void *sources[3];
    u8 source, target;
    u8 unknown16[2];
    u16 colors[16];
};
struct ShipWheel {
    struct Task task;
    struct ContactSwarmSprite sprite;
    struct ContactSwarmGraphics graphics;
    struct DisplaySequenceState sequence;
    void *sequenceResource;
    struct SceneEventReceiver receiver;
    u8 receiverState[0xC];
    void *owner;
    struct WheelPaletteTransition palette;
};
struct SceneFadePosition {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct SceneFade {
    struct Task task;
    struct ContactImpactEffect *target;
    struct SceneFadePosition from, to;
    u32 exitScript;
    void (*update)(struct SceneFade *);
    u16 timer;
    s16 wait;
    s16 firstDuration, secondDuration, finalDuration;
};
struct FallingSceneSprite {
    struct Task task;
    struct ProjectedDisplayEntry animation;
    struct ProjectedDisplayResource resource;
    struct SceneEventReceiver receiver;
    u8 receiverState[0xC];
    s32 velocityY;
    u32 id;
    void (*update)(struct FallingSceneSprite *);
    s32 timer;
};
void ScenePanelSubject_SlideOpen(struct ShipWheel *owner);
void ShipWheel_ProcessMessages(struct ShipWheel *obj);
void ShipWheel_Update(struct ShipWheel *wheel);
void ShipWheel_OnDestroy(struct ShipWheel *wheel);
s32 ShipWheel_Initialize(struct ShipWheel *wheel, void *owner);
void ShipWheel_RequestStop(void);
void *ShipWheel_Create(void *a);
void SceneTransition_Finish(struct SceneFade *fade);
void SceneTransition_StartFadeOut(struct SceneFade *fade);
void SceneTransition_StartCompletionDelay(struct SceneFade *fade);
void SceneFade_BeginSoundFade(struct SceneFade *fade);
void SceneFade_BeginTargetMove(struct SceneFade *fade);
void SceneTransition_StartChildTransition(struct SceneFade *fade);
void SceneFade_BeginSecondMove(struct SceneFade *fade);
void SceneFade_BeginFinalMove(struct SceneFade *fade);
void RegistryC281_UpdateOpeningDelay(struct SceneFade *fade);
void RegistryC281_UpdateFirstMovement(struct SceneFade *fade);
void RegistryC281_UpdateRepositioning(struct SceneFade *fade);
void RegistryC281_UpdateSecondMovement(struct SceneFade *fade);
void RegistryC281_UpdateFinalMovement(struct SceneFade *fade);
void RegistryC281_UpdateClosingDelay(struct SceneFade *fade);
void RegistryC281_UpdateClosingFade(struct SceneFade *fade);
void RegistryC281_Update(struct SceneFade *fade);
void RegistryC281_Destroy(void *fade);
s32 SceneFade_Initialize(struct SceneFade *fade);
void *RegistryC281_Create(void);
s32 Registry1909_UpdateVisibilityFlicker(struct ProjectedDisplayEntry *actor, s32 a, s32 b);
void Registry1909_EnterHiddenState(struct FallingSceneSprite *actor);
void Registry1909_StartVisibilityFlicker(struct FallingSceneSprite *actor);
void Registry1909_StartLandingAnimation(struct FallingSceneSprite *actor);
void Registry1909_StartFalling(struct FallingSceneSprite *actor);
void Registry1909_UpdateHiddenState(struct FallingSceneSprite *actor);
void Registry1909_UpdateLandingAnimation(struct FallingSceneSprite *actor);
void Registry1909_UpdateFlickerState(struct FallingSceneSprite *actor);
void Registry1909_UpdateFalling(struct FallingSceneSprite *actor);
void FallingSceneSprite_ProcessMessages(struct FallingSceneSprite *actor);
void Registry1909_Update(struct FallingSceneSprite *actor);
void Registry1909_Destroy(struct FallingSceneSprite *actor);
s32 FallingSceneSprite_Initialize(struct FallingSceneSprite *actor, u32 id);
void *SceneMessageSprite_Create(void *a);
void TimePalette_InitTransition(void *, const void *, const void *, const void *);
void SceneMessages_RegisterReceiver(void *, u32, s32);
void Camera_SetOffset(s32, void *);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
#endif
