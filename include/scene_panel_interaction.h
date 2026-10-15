#ifndef GUARD_SCENE_PANEL_INTERACTION_H
#define GUARD_SCENE_PANEL_INTERACTION_H

#include "scene_panel_rig.h"
#include "tether_actor.h"

/* Tentative views for the adjoining player/subject interaction state machine. */
struct ScenePanelInteractionPlayer {
    u8 unknown0[0x1C];
    u32 kindValue;
    u8 unknown20[0x10];
    struct ContactPosition position;
    u8 unknown38[0x3E0];
    u8 actionMode;
    u8 unknown419[0x3E];
    u8 tetherMode;
};
struct ScenePanelInteractionPackedPosition {
    u32 x : 16;
    u32 y : 16;
    u32 z : 16;
};
struct ScenePanelInteractionSubject {
    u8 unknown0[0xD3];
    u8 state;
    u8 unknownD4[0x1C];
    u8 changed;
};
struct ScenePanelInteractionView {
    u8 unknown0[0x1A];
    u16 initialMode;
    u8 unknown1C[2];
    u8 subjectCount;
    u8 unknown1F[0x231];
    void (*presentationState)(struct ScenePanel *);
    u32 mainSpriteFlags;
    u8 unknown258[0x2A4];
    u16 state, frame;
    struct ContactPosition origin, approachPosition, center;
    u8 unknown518[0x6D0];
    struct ScenePanelInteractionSubject *subjects[4];
    u16 resourceScale;
    u8 unknownBFA[0x15];
    u8 skipMainSprite;
    u8 mode, savedMode, gateC12;
    u8 unknownC13[3];
    u16 variant;
    u8 unknownC18[0x54];
    struct ScenePanelRig rig;
    u8 unknown1320[0x16];
    u8 allSubjectsNotified, arrivalRampEnabled;
    u16 arrivalRampFrame;
    u8 unknown133A[0x10];
    u16 transitionSound;
};

extern struct TetherActor *gUnk_02000488;
#define SCENE_PANEL_PLAYER ((struct ScenePanelInteractionPlayer *)gUnk_02000580[0])
s32 sub_0805B28C(const struct ContactPosition *, const struct ContactPosition *, s32);
s32 sub_0805B2CC(struct ScenePanel *);
s32 sub_0805B318(struct ScenePanel *);
s32 sub_0805B34C(struct ScenePanel *);
s32 sub_0805B3A0(struct ScenePanel *);
void ScenePanel_SetEffectState(struct ScenePanel *, u16);
void ScenePanel_SetState(struct ScenePanel *, void (*)(struct ScenePanel *));
void ScenePanel_BeginForwardSequence(struct ScenePanel *);
void ScenePanel_BeginLongSequence(struct ScenePanel *);
void ScenePanel_BeginPulseSequence(struct ScenePanel *);
void ScenePanel_RunScheduledAction(struct ScenePanel *);
void ScenePanelRig_SetState(struct ScenePanelRig *, u32);
void sub_0815B15C(struct ScenePanelInteractionPlayer *, const struct ContactPosition *, s32, u32,
                  u32);
void sub_0815AF14(struct ScenePanelInteractionPlayer *, s32);
void sub_0815AF50(struct ScenePanelInteractionPlayer *, s32, u32);
void sub_0815B0A8(struct ScenePanelInteractionPlayer *, const struct ContactPosition *, u32, u32);
void sub_0815B1D0(struct ScenePanelInteractionPlayer *, u32, u32);
void sub_0815B28C(struct ScenePanelInteractionPlayer *, u32);
void sub_0815B2E0(struct ScenePanelInteractionPlayer *, s32, u32);
void sub_0815C4FC(struct ScenePanelInteractionPlayer *, u32);
void sub_0815C99C(struct ScenePanelInteractionPlayer *);
void TetherActor_SetInteractionTarget(const void *, u32);
void TetherActor_BeginAuxiliaryState(struct TetherActor *);
void TetherActor_BeginDirectedState(struct ContactPosition *);
void sub_082260A4(s32);
void sub_08226380(u32, const void *);
void sub_08226508(u32);
void sub_0822B058(u32);
#endif
