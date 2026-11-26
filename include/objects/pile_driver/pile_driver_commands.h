#ifndef GUARD_SCENE_PANEL_COMMANDS_H
#define GUARD_SCENE_PANEL_COMMANDS_H
#include "objects/pile_driver/scene_panel_orchestration.h"

struct ScenePanelCommandGlobals {
    u8 unknown0[0x24];
    u8 bank;
};
extern struct ScenePanelCommandGlobals *gSceneEventDispatchState;
u32 SceneMessages_SetPendingTrigger(void *, void *);
void SceneMessages_UnregisterReceiver(void *);
s32 SceneMessages_RegisterReceiver(void *, u32, u32);
void Camera_SetPosition(const void *);

void PileDriver_DispatchCommands(struct ScenePanel *);
void PileDriver_UnregisterReceiver(struct ScenePanel *);
void PileDriver_RegisterReceiver(struct ScenePanel *);
void PileDriver_ClearGlobal(void);
u32 PileDriver_IsPresent(void);
void PileDriver_SelectMode(struct ScenePanel *);
s32 PileDriver_CountActiveSubjects(struct ScenePanel *);
void PileDriver_UpdateApproachSprite(struct ScenePanel *);
void PileDriver_SubmitPlayerMidpoint(struct ScenePanel *);
#endif
