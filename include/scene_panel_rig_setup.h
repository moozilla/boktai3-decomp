#ifndef GUARD_SCENE_PANEL_RIG_SETUP_H
#define GUARD_SCENE_PANEL_RIG_SETUP_H

#include "scene_panel_rig.h"

/* Setup/render fields of each existing 0x68-byte rig sprite. */
struct SceneRigSetupSprite {
    u32 flags;
    u8 unknown4[3];
    u8 renderMode;
    u8 unknown8[8];
    u16 scale;
    u8 unknown12[0xA];
    struct ContactPosition position;
    u8 unknown24[8];
    u8 graphics[0x1C];
    u8 animation[0x10];
    void *animationResource;
    u8 unknown5C[8];
    u16 randomPhase, phase;
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
void sub_08059428(struct SceneRigContact *, struct SceneRigContact *, struct ScenePanelRig *);
void sub_080595AC(struct ScenePanelRig *, u32);
void sub_0805AD50(struct ScenePanelRig *, u32);
void sub_0805ADE0(struct ScenePanelRig *);
void sub_0805AE58(struct ScenePanelRig *);
void sub_0805AF88(struct ScenePanelRig *);
void sub_0805B098(struct ScenePanelRig *);
void sub_0805B0B4(struct ScenePanelRig *);

#endif
