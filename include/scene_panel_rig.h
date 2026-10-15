#ifndef GUARD_SCENE_PANEL_RIG_H
#define GUARD_SCENE_PANEL_RIG_H
#include "scene_panel.h"
#include "contact_actor.h"
struct SceneRigSprite {
    u32 flags;
    u8 unknown4[0x64];
};
struct SceneRigSubject {
    u8 unknown0[0xD3];
    u8 state;
    u8 unknownD4[0x12];
    u16 requested;
};
struct SceneRigParentView {
    u8 unknown0[0x1E];
    u8 subjectCount, activeCount;
    u8 unknown20[0xBC8];
    struct SceneRigSubject *subjects[4];
    u8 unknownBF8[0x18];
    u8 mode;
    u8 unknownC11[0x731];
    u8 activated;
};
struct ScenePanelRig {
    struct SceneRigSprite main, primary[8], secondary[6];
    struct ContactPosition position;
    u8 unknown620[0x54];
    struct ContactPosition origin;
    u16 state, stateFrame, angle, turnFrame;
    u8 unknown684[4];
    u16 distance, speed;
    u8 unknown68C[5];
    u8 contactDirection;
    u16 contactForce, contactEffect;
    u8 unknown696[0x1A];
    struct ScenePanel *parent;
};
struct SceneRigContact {
    u8 unknown0[0x34];
    u32 typeFlags, reactionFlags;
    u16 power, unknown3E;
    u8 unknown40[2], direction;
};
void sub_0805952C(struct ScenePanelRig *);
void sub_0805956C(struct ScenePanelRig *);
void sub_08059638(struct ScenePanelRig *);
u32 sub_08059728(struct ScenePanelRig *);
void sub_0805CB88(struct SceneRigSubject *, u32);
extern const s16 gUnk_086149C4[];
#endif
