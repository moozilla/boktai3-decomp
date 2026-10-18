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
struct ScenePanelApproachSprite {
    u32 flags;
    u8 unknown4[4];
    u8 scaleX, scaleY;
    u8 unknownA[6];
    u16 frame;
    u8 unknown12[0x56];
};
struct ScenePanelInteractionView {
    u8 unknown0[0x1A];
    u16 initialMode;
    u8 unknown1C[2];
    u8 subjectCount;
    u8 activeCount;
    u8 unknown20[0x230];
    void (*presentationState)(struct ScenePanel *);
    u32 mainSpriteFlags;
    u8 unknown258[0x2A4];
    u16 state, frame;
    struct ContactPosition origin, approachPosition, center;
    struct ScenePanelApproachSprite approachSprite;
    u8 unknown580[0x668];
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
s32 ScenePanel_IsWithinHorizontalRadius(const struct ContactPosition *,
                                        const struct ContactPosition *, s32);
s32 ScenePanel_CanAcquireTether(struct ScenePanel *);
s32 ScenePanel_AreSubjectsSettled(struct ScenePanel *);
s32 ScenePanel_CanAcceptPlayer(struct ScenePanel *);
s32 ScenePanel_IsTetherDistant(struct ScenePanel *);
void ScenePanel_SetEffectState(struct ScenePanel *, u16);
void ScenePanel_SetState(struct ScenePanel *, void (*)(struct ScenePanel *));
void ScenePanel_BeginForwardSequence(struct ScenePanel *);
void ScenePanel_BeginLongSequence(struct ScenePanel *);
void ScenePanel_BeginPulseSequence(struct ScenePanel *);
void ScenePanel_RunScheduledAction(struct ScenePanel *);
void ScenePanelRig_SetState(struct ScenePanelRig *, u32);
void PlayerCue_MoveTo(struct ScenePanelInteractionPlayer *, const struct ContactPosition *, s32,
                      u32, u32);
void PlayerCue_Stand(struct ScenePanelInteractionPlayer *, s32);
void PlayerCue_Face(struct ScenePanelInteractionPlayer *, s32, u32);
void PlayerCue_WalkTo(struct ScenePanelInteractionPlayer *, const struct ContactPosition *, u32,
                      u32);
void PlayerCue_Motion7Front(struct ScenePanelInteractionPlayer *, u32, u32);
void PlayerCue_Motion7End(struct ScenePanelInteractionPlayer *, u32);
void PlayerCue_Kneel(struct ScenePanelInteractionPlayer *, s32, u32);
void PlayerCue_StateMotion(struct ScenePanelInteractionPlayer *, u32);
void sub_0815C99C(struct ScenePanelInteractionPlayer *);
void TetherActor_SetInteractionTarget(const void *, u32);
void TetherActor_BeginAuxiliaryState(struct TetherActor *);
void TetherActor_BeginDirectedState(struct ContactPosition *);
void sub_082260A4(s32);
void sub_08226380(u32, const void *);
void sub_08226508(u32);
void sub_0822B058(u32);
#endif
