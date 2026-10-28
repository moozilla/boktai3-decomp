#ifndef GUARD_SCENE_PANEL_COMMANDS_H
#define GUARD_SCENE_PANEL_COMMANDS_H
#include "scene_panel_orchestration.h"

struct ScenePanelCommandGlobals {
    u8 unknown0[0x24];
    u8 bank;
};
extern struct ScenePanelCommandGlobals *gSceneEventDispatchState;
u32 SceneMessages_SetPendingTrigger(void *, void *);
void SceneMessages_UnregisterReceiver(void *);
s32 SceneMessages_RegisterReceiver(void *, u32, u32);
void Camera_SetPosition(const void *);

void ScenePanel_DispatchCommands(struct ScenePanel *);
void ScenePanel_UnregisterReceiver(struct ScenePanel *);
void ScenePanel_RegisterReceiver(struct ScenePanel *);
void sub_0805C754(void);
u32 ScenePanel_IsPresent(void);
void ScenePanel_SelectMode(struct ScenePanel *);
s32 ScenePanel_CountActiveSubjects(struct ScenePanel *);
void ScenePanel_UpdateApproachSprite(struct ScenePanel *);
void ScenePanel_SubmitPlayerMidpoint(struct ScenePanel *);
#endif
