#ifndef GUARD_SCENE_PANEL_INTERACTION_H
#define GUARD_SCENE_PANEL_INTERACTION_H

#include "objects/pile_driver/pile_driver_rig.h"
#include "objects/coffin/tether_actor.h"

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

#define SCENE_PANEL_PLAYER ((struct ScenePanelInteractionPlayer *)gPlayers[0])
s32 PileDriver_IsWithinHorizontalRadius(const struct ContactPosition *,
                                        const struct ContactPosition *, s32);
s32 PileDriver_CanAcquireTether(struct ScenePanel *);
s32 PileDriver_AreSubjectsSettled(struct ScenePanel *);
s32 PileDriver_CanAcceptPlayer(struct ScenePanel *);
s32 PileDriver_IsTetherDistant(struct ScenePanel *);
void PileDriver_SetEffectState(struct ScenePanel *, u16);
void PileDriver_SetState(struct ScenePanel *, void (*)(struct ScenePanel *));
void PileDriver_BeginForwardSequence(struct ScenePanel *);
void PileDriver_BeginLongSequence(struct ScenePanel *);
void PileDriver_BeginPulseSequence(struct ScenePanel *);
void PileDriver_RunScheduledAction(struct ScenePanel *);
void PileDriverRig_SetState(struct ScenePanelRig *, u32);
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
void PlayerCue_ReleaseControl(struct ScenePanelInteractionPlayer *);
void TetherActor_SetInteractionTarget(const void *, u32);
void TetherActor_BeginAuxiliaryState(struct TetherActor *);
void TetherActor_BeginDirectedState(struct ContactPosition *);
void Camera_RequestShake(s32);
void sub_08226380(u32, const void *);
void Camera_FollowPlayerWithoutCompletion(u32);
void Sound_FadeOutChannel10(u32);
#endif
