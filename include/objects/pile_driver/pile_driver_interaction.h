#ifndef GUARD_PILE_DRIVER_INTERACTION_H
#define GUARD_PILE_DRIVER_INTERACTION_H

#include "objects/pile_driver/pile_driver_rig.h"
#include "objects/coffin/tether_actor.h"

/* Tentative views for the adjoining player/subject interaction state machine. */
struct PileDriverInteractionPlayer {
    u8 unknown0[0x1C];
    u32 kindValue;
    u8 unknown20[0x10];
    struct ContactPosition position;
    u8 unknown38[0x3E0];
    u8 actionMode;
    u8 unknown419[0x3E];
    u8 tetherMode;
};
struct PileDriverInteractionPackedPosition {
    u32 x : 16;
    u32 y : 16;
    u32 z : 16;
};

#define SCENE_PANEL_PLAYER ((struct PileDriverInteractionPlayer *)gPlayers[0])
s32 PileDriver_IsWithinHorizontalRadius(const struct ContactPosition *,
                                        const struct ContactPosition *, s32);
s32 PileDriver_CanAcquireTether(struct PileDriver *);
s32 PileDriver_AreSubjectsSettled(struct PileDriver *);
s32 PileDriver_CanAcceptPlayer(struct PileDriver *);
s32 PileDriver_IsTetherDistant(struct PileDriver *);
void PileDriver_SetEffectState(struct PileDriver *, u16);
void PileDriver_SetState(struct PileDriver *, void (*)(struct PileDriver *));
void PileDriver_BeginForwardSequence(struct PileDriver *);
void PileDriver_BeginLongSequence(struct PileDriver *);
void PileDriver_BeginPulseSequence(struct PileDriver *);
void PileDriver_RunScheduledAction(struct PileDriver *);
void PileDriverRig_SetState(struct PileDriverRig *, u32);
void PlayerCue_MoveTo(struct PileDriverInteractionPlayer *, const struct ContactPosition *, s32,
                      u32, u32);
void PlayerCue_Stand(struct PileDriverInteractionPlayer *, s32);
void PlayerCue_Face(struct PileDriverInteractionPlayer *, s32, u32);
void PlayerCue_WalkTo(struct PileDriverInteractionPlayer *, const struct ContactPosition *, u32,
                      u32);
void PlayerCue_Motion7Front(struct PileDriverInteractionPlayer *, u32, u32);
void PlayerCue_Motion7End(struct PileDriverInteractionPlayer *, u32);
void PlayerCue_Kneel(struct PileDriverInteractionPlayer *, s32, u32);
void PlayerCue_StateMotion(struct PileDriverInteractionPlayer *, u32);
void PlayerCue_ReleaseControl(struct PileDriverInteractionPlayer *);
void TetherActor_SetInteractionTarget(const void *, u32);
void TetherActor_BeginAuxiliaryState(struct TetherActor *);
void TetherActor_BeginDirectedState(struct ContactPosition *);
void Camera_RequestShake(s32);
void sub_08226380(u32, const void *);
void Camera_FollowPlayerWithoutCompletion(u32);
void Sound_FadeOutChannel10(u32);
#endif
