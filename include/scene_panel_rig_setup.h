#ifndef GUARD_SCENE_PANEL_RIG_SETUP_H
#define GUARD_SCENE_PANEL_RIG_SETUP_H

#include "scene_panel_rig.h"

struct SceneRigPayloadView {
    u8 unknown0[0x6A0];
    u16 firstX, firstY, firstZ, unknown6A6;
    u16 secondX, secondY, secondZ;
};

struct SceneRigSetupParentView {
    u8 unknown0[0x510];
    struct ContactPosition center;
    u8 unknown518[0x754];
    struct ScenePanelRig rig;
};

extern const char gUnk_0824DEAC[], gUnk_0824DEB0[];
u32 sub_08219BD8(const char *);
void sub_0821FF7C(void *, u32, u32, u32);
void ScenePanelRig_ApplyContact(struct SceneRigContact *, struct SceneRigContact *,
                                struct ScenePanelRig *);
void ScenePanelRig_SetState(struct ScenePanelRig *, u32);
void ScenePanelRig_InitCollider(struct ScenePanelRig *, u32);
void ScenePanelRig_InitMainSprite(struct ScenePanelRig *);
void ScenePanelRig_InitPrimarySprites(struct ScenePanelRig *);
void ScenePanelRig_InitSecondarySprites(struct ScenePanelRig *);
void ScenePanelRig_InitSprites(struct ScenePanelRig *);
void ScenePanelRig_InitPayloadVectors(struct ScenePanelRig *);

#endif
