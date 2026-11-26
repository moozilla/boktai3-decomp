#ifndef GUARD_SCENE_PANEL_RIG_SETUP_H
#define GUARD_SCENE_PANEL_RIG_SETUP_H

#include "objects/pile_driver/pile_driver_rig.h"

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
u32 String_Hash(const char *);
void Collider_SetBodyProperties(void *, u32, u32, u32);
void PileDriverRig_ApplyContact(struct ContactCollider *, struct ContactCollider *,
                                struct ScenePanelRig *);
void PileDriverRig_SetState(struct ScenePanelRig *, u32);
void PileDriverRig_InitCollider(struct ScenePanelRig *, u32);
void PileDriverRig_InitMainSprite(struct ScenePanelRig *);
void PileDriverRig_InitPrimarySprites(struct ScenePanelRig *);
void PileDriverRig_InitSecondarySprites(struct ScenePanelRig *);
void PileDriverRig_InitSprites(struct ScenePanelRig *);
void PileDriverRig_InitPayloadVectors(struct ScenePanelRig *);

#endif
