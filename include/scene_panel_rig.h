#ifndef GUARD_SCENE_PANEL_RIG_H
#define GUARD_SCENE_PANEL_RIG_H
#include "scene_panel.h"
#include "contact_actor.h"

struct SceneRigParentView {
    u8 unknown0[0x1E];
    u8 subjectCount, activeCount;
    u8 unknown20[0xBC8];
    struct ScenePanelSubject *subjects[4];
    u16 resourceScale;
    u8 unknownBFA[0x16];
    u8 mode;
    u8 unknownC11[0x731];
    u8 activated, warningStage;
    u16 warningSounds[4];
};

struct SceneRigContact {
    u8 unknown0[0x34];
    u32 typeFlags, reactionFlags;
    u16 power, unknown3E;
    u8 unknown40[2], direction;
};
u32 ScenePanelRig_IsSubjectState(struct ScenePanelRig *, u32, u32);
void ScenePanelRig_SetSubjectState(struct ScenePanelRig *, u32, u32);
void ScenePanelRig_RequestSubject(struct ScenePanelRig *, u32, u32);
void ScenePanelRig_SetState(struct ScenePanelRig *, u32);
void ScenePanelRig_RandomizeMovement(struct ScenePanelRig *);
s32 ScenePanelRig_SelectActiveSubject(struct ScenePanelRig *);
u32 ScenePanelRig_GetParentMode(struct ScenePanelRig *);
void ScenePanelRig_HideSprites(struct ScenePanelRig *);
void ScenePanelRig_ShowSprites(struct ScenePanelRig *);
void ScenePanelRig_InitMovement(struct ScenePanelRig *);
u32 ScenePanelRig_GetActiveCount(struct ScenePanelRig *);
void ScenePanelSubject_SetState(struct ScenePanelSubject *, u32);
extern const s16 gUnk_086149C4[];
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
#endif
