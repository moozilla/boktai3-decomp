#ifndef GUARD_SCENE_PRESENTATION_COMMANDS_H
#define GUARD_SCENE_PRESENTATION_COMMANDS_H
#include "orbiting_particles.h"
#include "scene_event_dispatch.h"
#include "scene_object.h"

/* Presentation messages extend the common eight-byte command header with operands. */
struct ScenePresentationMessage {
    u16 id;
    u8 receiverKind, trigger;
    u16 unknown4;
    u8 kind, unknown7;
    s16 arguments[9];
};
struct ScenePresentationReceiver {
    struct SceneEventReceiver messages;
    u32 unknown28;
    struct ScenePresentationReceiver *previous, *next;
};
struct ScenePresentation {
    struct Task task;
    struct ScenePresentationReceiver receiver;
    u32 unknown4C;
    s32 shakeFrames;
    u32 shakeStrength;
    struct OrbitingParticles *effect;
};
/* Script operands are packed into halfwords before being passed to effect creation. */
struct ScenePresentationPosition {
    u32 x : 16, y : 16, z : 16;
};
void SceneCommands_ClearEffect(struct ScenePresentation *, void *,
                               struct ScenePresentationMessage *);
void SceneCommands_CreateEffect(struct ScenePresentation *, void *,
                                struct ScenePresentationMessage *);
void SceneCommands_CreateOrbitingParticles(struct ScenePresentation *, void *,
                                           struct ScenePresentationMessage *);
void SceneCommands_Fade(struct ScenePresentation *, void *, struct ScenePresentationMessage *);
void SceneCommands_FadePalette(struct ScenePresentation *, void *,
                               struct ScenePresentationMessage *);
void SceneCommands_BlendPalette(struct ScenePresentation *, void *,
                                struct ScenePresentationMessage *);
void SceneCommands_SetShake(struct ScenePresentation *, void *, struct ScenePresentationMessage *);
void SceneCommands_StopShake(struct ScenePresentation *, void *, struct ScenePresentationMessage *);
void SceneCommands_ConfigureBlendWindow(struct ScenePresentation *, void *,
                                        struct ScenePresentationMessage *);
s32 SceneCommands_Dispatch(struct ScenePresentation *);
u32 SceneCommands_TaskUpdate(struct ScenePresentation *);
u32 SceneCommands_TaskDestroy(struct ScenePresentation *);
s32 SceneCommands_TaskInit(struct ScenePresentation *, u32);
struct ScenePresentation *SceneCommands_StartTask(u32);
struct SceneBody *SceneBody_GetById(s32);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void ColorFade_QueueCommand(s32, s32, void *);
void ColorFade_SetImmediate(s32, s32, s32);
s32 SceneMessages_RegisterReceiver(void *, u32, u32);
s32 SceneMessages_UnregisterReceiver(void *);
void Camera_RequestShake(u32);
#endif
