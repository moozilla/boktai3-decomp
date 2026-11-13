#ifndef GUARD_PLAYER_SCRIPT_CUES_H
#define GUARD_PLAYER_SCRIPT_CUES_H

#include "player_script.h"

void PlayerCue_Hide(struct Player *);
void ScriptActor_EnableCompletionScript(struct Player *);
void PlayerCue_OverlaySet(struct Player *, s32);
void PlayerCue_OverlayOn(struct Player *actor, s32 animation);
void PlayerCue_OverlayOff(struct Player *);
void PlayerCue_Stand(struct Player *, s32);
void PlayerCue_Face(struct Player *, s32, s32);
void PlayerCue_Idle(struct Player *, s32);
void PlayerCue_StandVariant2(struct Player *, s32, s32);
void PlayerCue_StandDiagonalB(struct Player *, s32, s32);
void PlayerCue_WalkTo(struct Player *actor, struct PlayerScriptPackedPoint *destination, s32 e, s32 direction);
void PlayerCue_RunTo(struct Player *actor, struct PlayerScriptPackedPoint *point, s32 direction, s32 e);
void PlayerCue_MoveTo(struct Player *actor, struct PlayerScriptPackedPoint *point, s32 direction, s32 f, s32 e);
void PlayerCue_Motion7Front(struct Player *actor, s32 f, s32 e);
void ScriptActor_CallCompletionScript(struct Player *);
void PlayerCue_Motion7End(struct Player *, s32);
void PlayerCue_Kneel(struct Player *, s32, s32);
void PlayerCue_KneelGasp(struct Player *, s32);
void PlayerCue_KneelDown(struct Player *, s32);
void PlayerCue_GetUp(struct Player *, s32, s32);
void PlayerCue_BeginMode27(struct Player *, s32, s32, s32);
void PlayerCue_BeginMode28(struct Player *actor, s32 direction, s32 h, s32 e);
void PlayerCue_Motion29(struct Player *, s32, s32, s32);
void PlayerCue_Motion30(struct Player *, s32, s32, s32, s32);
void PlayerCue_Motion30End(struct Player *, s32);
void PlayerCue_Motion29B(struct Player *, s32, s32, s32);
void PlayerCue_BeginOddDirection(struct Player *, s32);
void PlayerCue_Motion7Turn(struct Player *, s32, s32);
void PlayerCue_Motion7Variant3(struct Player *, s32, s32);
void PlayerCue_AimDraw(struct Player *, s32, s32);
void PlayerCue_AimDrawVariant2(struct Player *, s32, s32);
void PlayerCue_AimSteady(struct Player *, s32, s32);
void PlayerCue_AimSteadyVariant2(struct Player *, s32, s32);
void PlayerCue_SetAimPose(struct Player *, s32, s32);
void PlayerCue_SetFiringPose(struct Player *, s32, s32);
void PlayerCue_Aim(struct Player *, s32, s32);
void PlayerCue_Shoot(struct Player *, s32, s32);
s32 PlayerCue_FormMotion(struct Player *, s32);
void PlayerCue_Windblown(struct Player *, s32, s32);
void PlayerCue_WindblownVariant3(struct Player *, s32, s32);
void PlayerCue_WindblownVariant4(struct Player *, s32, s32);
void PlayerCue_WindblownVariant2(struct Player *, s32, s32);
void PlayerCue_StandDiagonal(struct Player *, s32, s32);
void PlayerCue_FaceSide(struct Player *, s32);
void Player_ConfigureAttachmentEffect(struct Player *actor, s32 mode, s32 c, s32 a);
void PlayerCue_ReleaseControl(struct Player *);
void Hud_RegisterTimer(u16 *);
void Player_EnterKindTwo(struct Player *);
void PlayerCue_UpdateTimedChargePulse(struct Player *);
void Player_UpdateTimedChargeAction(struct Player *);

#endif
