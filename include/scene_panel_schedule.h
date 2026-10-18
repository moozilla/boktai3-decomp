#ifndef GUARD_SCENE_PANEL_SCHEDULE_H
#define GUARD_SCENE_PANEL_SCHEDULE_H
#include "scene_panel.h"
#include "contact_actor.h"
/* Observed scheduled action view of the same ScenePanel; identities tentative. */
struct ScenePanelScheduleView {
    u8 unknown0[0x500];
    struct ContactPosition origin;
    u8 unknown508[0xE32];
    u8 resourceIndex;
    u8 unknown133B[0x11];
    u16 frame, interval, step, resourceId, unknown1354, angle;
    u8 unknown1358[8];
    void (*action)(struct ScenePanel *);
    void *handle;
    u8 unknown1368[0x5C];
    s16 parameter;
};
void *AnimatedHazard_Create(const void *, s32, s32);
void DirectionBurst_Emit(const void *, u32, u32, u32);
void *Hresvelgr_CreateShotPool(u16, u32, u32);
void *SceneEffect_CreateMovingTask(u16, const void *);
void *ScenePanelEffect_Create(u16, s32);
void *SceneEffect_CreateEntryTask(u16, const void *);
void SceneBurstEmitter_RequestCleanup(void *);
void SceneOrbitHazard_Activate(void *);
void sub_08190B10(void *, const void *, const void *);
extern const u8 gUnk_0824DEA4[];
void ScenePanel_TickHazardSpawner(struct ScenePanel *);
void ScenePanel_TickAngleBurst(struct ScenePanel *);
void ScenePanel_TickHandlePosition(struct ScenePanel *);
void ScenePanel_TickHandleActionA(struct ScenePanel *);
void ScenePanel_TickHandleActionB(struct ScenePanel *);
#endif
