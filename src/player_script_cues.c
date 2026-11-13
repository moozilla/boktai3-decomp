#include "player_script_cues.h"

s32 ScriptActor_GetDirectionFromScript(void)
{
    s32 result;
    if (Script_SeekToKeyword(0x64) == 0)
        result = -1;
    else
        result = Script_GetValue();
    return result;
}

s32 ScriptActor_GetCueOptionFromScript(void)
{
    if (Script_SeekToKeyword(0x65))
        return Script_GetValue();
    return 0;
}

void ScriptActor_HideFromScript(void)
{
    struct Player *player;
    s32 index = Script_GetActorIndex();
    player = gPlayers[index];
    if (player) {
        PlayerCue_Hide(player);
        ScriptActor_EnableCompletionScript(player);
    }
}

void ScriptActor_RequestFromScript(void)
{
    s32 index = Script_GetActorIndex();
    struct Player *actor = gPlayers[index];
    if (actor && Script_SeekToKeyword('p')) {
        PlayerCue_OverlaySet(actor, Script_GetValue() + 0x16E);
        ScriptActor_EnableCompletionScript(actor);
    }
}

void ScriptActor_SetAnimationFromScript(void)
{
    struct Player *actor;
    s32 index = Script_GetActorIndex();
    actor = gPlayers[index];
    if (actor && Script_SeekToKeyword('p')) {
        s32 animation = Script_GetValue();
        PlayerCue_OverlayOn(actor, animation + 366);
        ScriptActor_EnableCompletionScript(actor);
    }
}

void ScriptActor_DisableOverlayFromScript(void)
{
    struct Player *player;
    s32 index = Script_GetActorIndex();
    player = gPlayers[index];
    if (player) {
        PlayerCue_OverlayOff(player);
        ScriptActor_EnableCompletionScript(player);
    }
}

void ScriptActor_StandFromScript(void)
{
    struct Player *player;
    s32 index = Script_GetActorIndex();
    player = gPlayers[index];
    if (player != 0) {
        PlayerCue_Stand(player, ScriptActor_GetDirectionFromScript());
        ScriptActor_EnableCompletionScript(player);
    }
}

void ScriptActor_SetFacingFromScript(void)
{
    struct Player *player;
    s32 index = Script_GetActorIndex();
    s32 direction;
    player = gPlayers[index];
    if (player != 0) {
        direction = ScriptActor_GetDirectionFromScript();
        PlayerCue_Face(player, direction, ScriptActor_GetCueOptionFromScript());
        ScriptActor_EnableCompletionScript(player);
    }
}

void ScriptActor_IdleFromScript(void)
{
    struct Player *player;
    s32 index = Script_GetActorIndex();
    player = gPlayers[index];
    if (player != 0) {
        PlayerCue_Idle(player, ScriptActor_GetDirectionFromScript());
        ScriptActor_EnableCompletionScript(player);
    }
}

void ScriptActor_StandVariant2FromScript(void)
{
    struct Player *player;
    s32 index = Script_GetActorIndex();
    s32 direction;
    player = gPlayers[index];
    if (player != 0) {
        direction = ScriptActor_GetDirectionFromScript();
        PlayerCue_StandVariant2(player, direction, ScriptActor_GetCueOptionFromScript());
        ScriptActor_EnableCompletionScript(player);
    }
}

void ScriptActor_StandDiagonalBFromScript(void)
{
    struct Player *player;
    s32 index = Script_GetActorIndex();
    s32 direction;
    player = gPlayers[index];
    if (player != 0) {
        direction = ScriptActor_GetDirectionFromScript();
        PlayerCue_StandDiagonalB(player, direction, ScriptActor_GetCueOptionFromScript());
        ScriptActor_EnableCompletionScript(player);
    }
}

void ScriptActor_MoveToFromScript(void)
{
    struct PlayerScriptPackedPoint destination;
    struct Player *actor;
    s32 index = Script_GetActorIndex();
    actor = gPlayers[index];
    if (actor && Script_SeekToKeyword('p')) {
        s32 direction, cueOption;
        destination.x = Script_GetValue();
        destination.y = Script_GetValue();
        destination.z = Script_GetValue();
        direction = ScriptActor_GetDirectionFromScript();
        cueOption = ScriptActor_GetCueOptionFromScript();
        PlayerCue_WalkTo(actor, &destination, cueOption, direction);
        ScriptActor_EnableCompletionScript(actor);
    }
}

void ScriptActor_RunToFromScript(void)
{
    struct PlayerScriptPackedPoint point;
    struct Player *actor;
    s32 index = Script_GetActorIndex();
    actor = gPlayers[index];
    if (actor && Script_SeekToKeyword('p')) {
        s32 direction, cueOption;
        point.x = Script_GetValue();
        point.y = Script_GetValue();
        point.z = Script_GetValue();
        direction = ScriptActor_GetDirectionFromScript();
        cueOption = ScriptActor_GetCueOptionFromScript();
        PlayerCue_RunTo(actor, &point, direction, cueOption);
        ScriptActor_EnableCompletionScript(actor);
    }
}

void ScriptActor_TurnFromScript(void)
{
    struct PlayerScriptPackedPoint point;
    struct Player *actor;
    s32 index = Script_GetActorIndex();
    actor = gPlayers[index];
    if (actor) {
        s32 motionOption, direction, cueOption;
        if (Script_SeekToKeyword('f'))
            motionOption = Script_GetValue();
        else
            motionOption = 0;
        direction = ScriptActor_GetDirectionFromScript();
        cueOption = ScriptActor_GetCueOptionFromScript();
        if (Script_SeekToKeyword('p')) {
            point.x = Script_GetValue();
            point.y = Script_GetValue();
            point.z = Script_GetValue();
            PlayerCue_MoveTo(actor, &point, direction, motionOption, cueOption);
        } else
            PlayerCue_MoveTo(actor, 0, direction, motionOption, cueOption);
        ScriptActor_EnableCompletionScript(actor);
    }
}

void ScriptActor_EnterStateFourFromScript(void)
{
    struct Player *actor;
    s32 index = Script_GetActorIndex();
    actor = gPlayers[index];
    if (actor) {
        s32 cueOption = ScriptActor_GetCueOptionFromScript();
        u8 state = actor->actionMode;
        if (state <= 1 || state == 6) {
            s32 motionOption; /* meaning unknown */
            if (Script_SeekToKeyword('f'))
                motionOption = Script_GetValue();
            else
                motionOption = 0;
            PlayerCue_Motion7Front(actor, motionOption, cueOption);
            ScriptActor_EnableCompletionScript(actor);
        } else {
            actor->cueParameter = cueOption;
            PlayerCue_Stand(actor, 4);
            ScriptActor_EnableCompletionScript(actor);
            ScriptActor_CallCompletionScript(actor);
        }
    }
}

void ScriptActor_EndMotion7FromScript(void)
{
    struct Player *player;
    s32 index = Script_GetActorIndex();
    player = gPlayers[index];
    if (player != 0) {
        PlayerCue_Motion7End(player, ScriptActor_GetCueOptionFromScript());
        ScriptActor_EnableCompletionScript(player);
    }
}

void ScriptActor_KneelFromScript(void)
{
    struct Player *player;
    s32 index = Script_GetActorIndex();
    s32 direction;
    player = gPlayers[index];
    if (player != 0) {
        direction = ScriptActor_GetDirectionFromScript();
        PlayerCue_Kneel(player, direction, ScriptActor_GetCueOptionFromScript());
        ScriptActor_EnableCompletionScript(player);
    }
}

void ScriptActor_KneelGaspFromScript(void)
{
    struct Player *player;
    s32 index = Script_GetActorIndex();
    player = gPlayers[index];
    if (player != 0) {
        PlayerCue_KneelGasp(player, ScriptActor_GetDirectionFromScript());
        ScriptActor_EnableCompletionScript(player);
    }
}

void ScriptActor_KneelDownFromScript(void)
{
    struct Player *player;
    s32 index = Script_GetActorIndex();
    player = gPlayers[index];
    if (player != 0) {
        PlayerCue_KneelDown(player, ScriptActor_GetDirectionFromScript());
        ScriptActor_EnableCompletionScript(player);
    }
}

void ScriptActor_GetUpFromScript(void)
{
    struct Player *player;
    s32 index = Script_GetActorIndex();
    s32 direction;
    player = gPlayers[index];
    if (player != 0) {
        direction = ScriptActor_GetDirectionFromScript();
        PlayerCue_GetUp(player, direction, ScriptActor_GetCueOptionFromScript());
        ScriptActor_EnableCompletionScript(player);
    }
}

void ScriptActor_FaceFromScript(void)
{
    s32 index = Script_GetActorIndex();
    struct Player *actor = gPlayers[index];
    if (actor) {
        s32 direction = ScriptActor_GetDirectionFromScript();
        s32 cueOption = ScriptActor_GetCueOptionFromScript();
        s32 motionOption;
        if (Script_SeekToKeyword('f'))
            motionOption = Script_GetValue();
        else
            motionOption = 0;
        PlayerCue_BeginMode27(actor, direction, motionOption, cueOption);
    }
}

void ScriptActor_StartActionFromScript(void)
{
    struct Player *actor;
    s32 index = Script_GetActorIndex();
    actor = gPlayers[index];
    if (actor) {
        s32 direction = ScriptActor_GetDirectionFromScript();
        s32 cueOption = ScriptActor_GetCueOptionFromScript();
        s32 height;
        if (Script_SeekToKeyword('h'))
            height = Script_GetValue();
        else
            height = 1500;
        PlayerCue_BeginMode28(actor, direction, height, cueOption);
        ScriptActor_EnableCompletionScript(actor);
    }
}

void ScriptActor_PlayMotion29FromScript(void)
{
    s32 index = Script_GetActorIndex();
    struct Player *player = gPlayers[index];
    s32 direction, motionOption;
    if (player != 0) {
        if (Script_SeekToKeyword(0x66) != 0)
            motionOption = Script_GetValue();
        else
            motionOption = 0x1e;
        direction = ScriptActor_GetDirectionFromScript();
        PlayerCue_Motion29(player, direction, motionOption, ScriptActor_GetCueOptionFromScript());
        ScriptActor_EnableCompletionScript(player);
    }
}

void ScriptActor_PlayMotion30FromScript(void)
{
    s32 index = Script_GetActorIndex();
    struct Player *player = gPlayers[index];
    s32 direction, motionOption, lengthOption;
    if (player != 0) {
        if (Script_SeekToKeyword(0x66) != 0)
            motionOption = Script_GetValue();
        else
            motionOption = 0x32;
        if (Script_SeekToKeyword(0x4c) != 0)
            lengthOption = Script_GetValue();
        else
            lengthOption = 0;
        direction = ScriptActor_GetDirectionFromScript();
        PlayerCue_Motion30(player, direction, motionOption, lengthOption, ScriptActor_GetCueOptionFromScript());
        ScriptActor_EnableCompletionScript(player);
    }
}

void ScriptActor_EndMotion30FromScript(void)
{
    s32 index = Script_GetActorIndex();
    struct Player *player = gPlayers[index];
    if (player != 0) {
        PlayerCue_Motion30End(player, ScriptActor_GetCueOptionFromScript());
        ScriptActor_EnableCompletionScript(player);
    }
}

void ScriptActor_StartMotion29BFromScript(void)
{
    s32 index = Script_GetActorIndex();
    struct Player *player = gPlayers[index];
    s32 direction, motionOption;
    if (player != 0) {
        if (Script_SeekToKeyword(0x66) != 0)
            motionOption = Script_GetValue();
        else
            motionOption = 0x32;
        direction = ScriptActor_GetDirectionFromScript();
        PlayerCue_Motion29B(player, direction, motionOption, ScriptActor_GetCueOptionFromScript());
        ScriptActor_EnableCompletionScript(player);
    }
}

void ScriptActor_StartMotionB774FromScript(void)
{
    s32 index = Script_GetActorIndex();
    struct Player *player = gPlayers[index];
    if (player != 0) {
        PlayerCue_BeginOddDirection(player, ScriptActor_GetDirectionFromScript());
        ScriptActor_EnableCompletionScript(player);
    }
}

void ScriptActor_TurnMotion7FromScript(void)
{
    s32 index = Script_GetActorIndex();
    struct Player *player = gPlayers[index];
    s32 direction;
    if (player != 0) {
        direction = ScriptActor_GetDirectionFromScript();
        PlayerCue_Motion7Turn(player, direction, ScriptActor_GetCueOptionFromScript());
        ScriptActor_EnableCompletionScript(player);
    }
}

void ScriptActor_StartMotion7Variant3FromScript(void)
{
    s32 index = Script_GetActorIndex();
    struct Player *player = gPlayers[index];
    s32 direction;
    if (player != 0) {
        direction = ScriptActor_GetDirectionFromScript();
        PlayerCue_Motion7Variant3(player, direction, ScriptActor_GetCueOptionFromScript());
        ScriptActor_EnableCompletionScript(player);
    }
}

void ScriptActor_StartAimSequenceFromScript(void)
{
    s32 index = Script_GetActorIndex();
    struct Player *player = gPlayers[index];
    s32 direction;
    if (player != 0) {
        direction = ScriptActor_GetDirectionFromScript();
        PlayerCue_AimDraw(player, direction, ScriptActor_GetCueOptionFromScript());
        ScriptActor_EnableCompletionScript(player);
    }
}

void ScriptActor_StartAimSequenceVariant2FromScript(void)
{
    s32 index = Script_GetActorIndex();
    struct Player *player = gPlayers[index];
    s32 direction;
    if (player != 0) {
        direction = ScriptActor_GetDirectionFromScript();
        PlayerCue_AimDrawVariant2(player, direction, ScriptActor_GetCueOptionFromScript());
        ScriptActor_EnableCompletionScript(player);
    }
}

void ScriptActor_StartAimHoldAFromScript(void)
{
    s32 index = Script_GetActorIndex();
    struct Player *player = gPlayers[index];
    s32 direction;
    if (player != 0) {
        direction = ScriptActor_GetDirectionFromScript();
        PlayerCue_AimSteady(player, direction, ScriptActor_GetCueOptionFromScript());
        ScriptActor_EnableCompletionScript(player);
    }
}

void ScriptActor_StartAimHoldAVariant2FromScript(void)
{
    s32 index = Script_GetActorIndex();
    struct Player *player = gPlayers[index];
    s32 direction;
    if (player != 0) {
        direction = ScriptActor_GetDirectionFromScript();
        PlayerCue_AimSteadyVariant2(player, direction, ScriptActor_GetCueOptionFromScript());
        ScriptActor_EnableCompletionScript(player);
    }
}

void ScriptActor_StartAimHoldBFromScript(void)
{
    s32 index = Script_GetActorIndex();
    struct Player *player = gPlayers[index];
    s32 direction;
    if (player != 0) {
        direction = ScriptActor_GetDirectionFromScript();
        PlayerCue_SetAimPose(player, direction, ScriptActor_GetCueOptionFromScript());
        ScriptActor_EnableCompletionScript(player);
    }
}

void ScriptActor_StartAimHoldBVariant2FromScript(void)
{
    s32 index = Script_GetActorIndex();
    struct Player *player = gPlayers[index];
    s32 direction;
    if (player != 0) {
        direction = ScriptActor_GetDirectionFromScript();
        PlayerCue_SetFiringPose(player, direction, ScriptActor_GetCueOptionFromScript());
        ScriptActor_EnableCompletionScript(player);
    }
}

void ScriptActor_AimFromScript(void)
{
    s32 index = Script_GetActorIndex();
    struct Player *player = gPlayers[index];
    s32 direction;
    if (player != 0) {
        direction = ScriptActor_GetDirectionFromScript();
        PlayerCue_Aim(player, direction, ScriptActor_GetCueOptionFromScript());
        ScriptActor_EnableCompletionScript(player);
    }
}

void ScriptActor_ShootFromScript(void)
{
    s32 index = Script_GetActorIndex();
    struct Player *player = gPlayers[index];
    s32 direction;
    if (player != 0) {
        if (Script_SeekToKeyword(0x6c) != 0)
            direction = Script_GetValue();
        else
            direction = 0x20;
        PlayerCue_Shoot(player, direction, ScriptActor_GetCueOptionFromScript());
        ScriptActor_EnableCompletionScript(player);
    }
}

void ScriptActor_StartFormMotionFromScript(void)
{
    s32 index = Script_GetActorIndex();
    struct Player *player = gPlayers[index];
    s32 result;
    if (player != 0) {
        result = PlayerCue_FormMotion(player, ScriptActor_GetCueOptionFromScript());
        ScriptActor_EnableCompletionScript(player);
        if (result == 0)
            ScriptActor_CallCompletionScript(player);
    }
}

void ScriptActor_WindblownFromScript(void)
{
    s32 index = Script_GetActorIndex();
    struct Player *actor = gPlayers[index];
    if (actor != NULL) {
        s32 value;
        if (Script_SeekToKeyword('r'))
            value = Script_GetValue();
        else
            value = 0;
        PlayerCue_Windblown(actor, value, ScriptActor_GetCueOptionFromScript());
        ScriptActor_EnableCompletionScript(actor);
    }
}

void ScriptActor_WindblownVariant3FromScript(void)
{
    s32 index = Script_GetActorIndex();
    struct Player *actor = gPlayers[index];
    if (actor != NULL) {
        s32 value;
        if (Script_SeekToKeyword('r'))
            value = Script_GetValue();
        else
            value = 0;
        PlayerCue_WindblownVariant3(actor, value, ScriptActor_GetCueOptionFromScript());
        ScriptActor_EnableCompletionScript(actor);
    }
}

void ScriptActor_WindblownVariant4FromScript(void)
{
    s32 index = Script_GetActorIndex();
    struct Player *actor = gPlayers[index];
    if (actor != NULL) {
        s32 value;
        if (Script_SeekToKeyword('r'))
            value = Script_GetValue();
        else
            value = 0;
        PlayerCue_WindblownVariant4(actor, value, ScriptActor_GetCueOptionFromScript());
        ScriptActor_EnableCompletionScript(actor);
    }
}

void ScriptActor_WindblownVariant2FromScript(void)
{
    s32 index = Script_GetActorIndex();
    struct Player *actor = gPlayers[index];
    if (actor != NULL) {
        s32 value;
        if (Script_SeekToKeyword('r'))
            value = Script_GetValue();
        else
            value = 0;
        PlayerCue_WindblownVariant2(actor, value, ScriptActor_GetCueOptionFromScript());
        ScriptActor_EnableCompletionScript(actor);
    }
}

void ScriptActor_StandDiagonalFromScript(void)
{
    s32 index = Script_GetActorIndex();
    struct Player *actor = gPlayers[index];
    if (actor != NULL) {
        s32 value;
        if (Script_SeekToKeyword('d'))
            value = Script_GetValue();
        else
            value = 0;
        PlayerCue_StandDiagonal(actor, value, ScriptActor_GetCueOptionFromScript());
        ScriptActor_EnableCompletionScript(actor);
    }
}

void ScriptActor_FaceSideFromScript(void)
{
    s32 index = Script_GetActorIndex();
    struct Player *player = gPlayers[index];
    s32 direction;
    if (player != 0) {
        if (Script_SeekToKeyword(0x72) != 0)
            direction = Script_GetValue();
        else
            direction = 0;
        PlayerCue_FaceSide(player, direction);
        ScriptActor_EnableCompletionScript(player);
    }
}

void ScriptActor_SetModeFromScript(void)
{
    struct Player *actor;
    s32 index = Script_GetActorIndex();
    actor = gPlayers[index];
    if (actor && Script_SeekToKeyword('m')) {
        s32 mode = Script_GetValue();
        s32 countOption, amplitudeOption;
        if ((u32)(mode - 4) <= 1) {
            if (Script_SeekToKeyword('c'))
                countOption = Script_GetValue();
            else
                countOption = 120;
            if (Script_SeekToKeyword('A'))
                amplitudeOption = Script_GetValue();
            else
                amplitudeOption = 120;
        } else {
            amplitudeOption = 0;
            countOption = 0;
        }
        Player_ConfigureAttachmentEffect(actor, mode, countOption, amplitudeOption);
    }
}

void ScriptActor_ResetActionStateFromScript(void)
{
    s32 index = Script_GetActorIndex();
    struct Player *player = gPlayers[index];
    if (player != 0)
        PlayerCue_ReleaseControl(player);
}

void ScriptActor_StartTimedChargeFromScript(void)
{
    s32 index = Script_GetActorIndex();
    struct Player *actor = gPlayers[index];
    if (actor != NULL) {
        u16 *timer;
        s32 value;
        if (Script_SeekToKeyword('t')) {
            value = Script_GetValue();
            timer = &actor->timedChargeTimer;
        } else {
            timer = &actor->timedChargeTimer;
            value = 600;
        }
        *timer = value;
        Hud_RegisterTimer(&actor->timedChargeTimer);
        if (Script_SeekToKeyword('c')) {
            value = Script_GetValue();
            timer = &actor->timedChargePeriod;
        } else {
            timer = &actor->timedChargePeriod;
            value = 720;
        }
        *timer = value;
        actor->timedChargePhase = 0;
        actor->cueParameter = ScriptActor_GetCueOptionFromScript();
        actor->facing = 4;
        Player_UpdateFacingSide(actor);
        Player_EnterKindTwo(actor);
        ActorSequence_Begin(actor, 7, 0);
        if (actor->actionMode == 6)
            actor->triggerAnimationCallback = PlayerCue_UpdateTimedChargePulse;
        else
            actor->triggerAnimationCallback = Player_UpdateTimedChargeAction;
    }
}
