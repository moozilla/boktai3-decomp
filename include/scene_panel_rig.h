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
    u16 resourceScale;
    u8 unknownBFA[0x16];
    u8 mode;
    u8 unknownC11[0x731];
    u8 activated, warningStage;
    u16 warningSounds[4];
};
struct ScenePanelRig {
    struct SceneRigSprite main, primary[8], secondary[6];
    struct ContactPosition position;
    u8 unknown620[0x54];
    struct ContactPosition origin;
    u16 state, stateFrame, angle, turnFrame;
    s16 turnRate;
    u16 turnDuration;
    u16 distance, speed;
    u16 verticalStep, scale;
    u8 motionFlag;
    u8 contactDirection;
    u16 contactForce, contactEffect;
    u8 unknown696[4];
    u16 resourceAccumulator;
    u8 unknown69C[4];
    u8 payloadA[8], payloadB[8];
    struct ScenePanel *parent;
};
struct SceneRigContact {
    u8 unknown0[0x34];
    u32 typeFlags, reactionFlags;
    u16 power, unknown3E;
    u8 unknown40[2], direction;
};
void sub_080595AC(struct ScenePanelRig *, u32);
void sub_080597E0(struct ScenePanelRig *);
u32 sub_08059734(struct ScenePanelRig *);
void sub_0805952C(struct ScenePanelRig *);
void sub_0805956C(struct ScenePanelRig *);
void sub_08059638(struct ScenePanelRig *);
u32 sub_08059728(struct ScenePanelRig *);
void sub_0805CB88(struct SceneRigSubject *, u32);
extern const s16 gUnk_086149C4[];
void sub_082279A8(s32, s32, s32, s32, s32, s32, s32);
#endif
