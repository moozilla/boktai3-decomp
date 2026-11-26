#ifndef GUARD_PILE_DRIVER_COMMANDS_H
#define GUARD_PILE_DRIVER_COMMANDS_H
#include "objects/pile_driver/pile_driver_orchestration.h"

struct PileDriverCommandGlobals {
    u8 unknown0[0x24];
    u8 bank;
};
extern struct PileDriverCommandGlobals *gSceneEventDispatchState;
u32 SceneMessages_SetPendingTrigger(void *, void *);
void SceneMessages_UnregisterReceiver(void *);
s32 SceneMessages_RegisterReceiver(void *, u32, u32);
void Camera_SetPosition(const void *);

void PileDriver_DispatchCommands(struct PileDriver *);
void PileDriver_UnregisterReceiver(struct PileDriver *);
void PileDriver_RegisterReceiver(struct PileDriver *);
void PileDriver_ClearGlobal(void);
u32 PileDriver_IsPresent(void);
void PileDriver_SelectMode(struct PileDriver *);
s32 PileDriver_CountActiveSubjects(struct PileDriver *);
void PileDriver_UpdateApproachSprite(struct PileDriver *);
void PileDriver_SubmitPlayerMidpoint(struct PileDriver *);
#endif
