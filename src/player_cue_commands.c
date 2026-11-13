#include "player_cue_commands.h"

/* Enable the first player's stat gauge. */

s32 Player_EnableStatGaugeFromScript(void)
{
    gPlayers[0]->statGaugeFull = 1;
    return 0;
}

/* Enable the first player's stat gauge and history direction mode. */

s32 Player_EnableStatGaugeAndHistoryFromScript(void)
{
    struct Player *player = gPlayers[0];
    player->statGaugeFull = 1;
    player->historyDirectionMode = 1;
    return 0;
}

/* Restrict the first player's action requests. */

s32 Player_RestrictActionRequestsFromScript(void)
{
    gPlayers[0]->actionRequestRestricted = 1;
    return 0;
}

void Player_EnterKindTwo(struct Player *player)
{
    if (player->kind != 2) {
        player->kindTwoAttachmentCounter = 0;
        player->kindTwoAttachment = ActorAttachment_UpdateFromDescriptor(
            player->kindTwoAttachment, &player->positionalEffectGroup,
            player->kindTwoAttachmentDescriptor, 0x7F, 0, 0, 0, 0x50);
    }
    player->kind = 2;
    player->kindTwoState = 0;
    player->kindTwoEntered = 1;
    player->terrainRecoveryCounter = 0;
    player->actionCountdown = 0;
    if ((*(struct PlayerPendingNotification **)&gPlayerPendingActor) &&
        gPlayerNotificationPending) {
        if (!((*(struct PlayerPendingNotification **)&gPlayerPendingActor)->flags & 1)) {
            if ((*(struct PlayerPendingNotification **)&gPlayerPendingActor)->flags & 0x10)
                (*(struct PlayerPendingNotification **)&gPlayerPendingActor)
                    ->callback(
                        (*(struct PlayerPendingNotification **)&gPlayerPendingActor)->receiver, 1,
                        60);
            else
                (*(struct PlayerPendingNotification **)&gPlayerPendingActor)
                    ->callback(
                        (*(struct PlayerPendingNotification **)&gPlayerPendingActor)->receiver, 1,
                        60);
        }
        (*(struct PlayerPendingNotification **)&gPlayerPendingActor) = 0;
        gPlayerNotificationPending = 0;
    }
    Player_StopActionSounds(player);
    player->directionFrame = 0;
    player->collider.flags |= 4;
    player->positionResetValue = 0;
    if (player->cameraOffsetActive != 0)
        Player_RestoreCameraFollow(player);
    Player_AddControlFlags(player, 8);
    {
        u32 flag = 0x40000;
        player->statusFlags |= flag;
    }
}

void PlayerCue_OverlaySet(struct Player *player, s32 parameter)
{
    player->visualEffectMode = 1;
    player->actionEighteenParameter = parameter;
}

void PlayerCue_OverlayOn(struct Player *player, s32 parameter)
{
    player->visualEffectMode = 2;
    player->actionEighteenParameter = parameter;
}

void PlayerCue_OverlayOff(struct Player *player)
{
    player->visualEffectMode = 0;
}

void PlayerCue_SetPosition(struct Player *player, const struct ContactPosition *position)
{
    (*(struct ContactPosition *)&player->position) = *position;
}

void PlayerCue_Hide(struct Player *player)
{
    Player_EnterKindTwo(player);
    ActorSequence_Begin(player, 0, 0);
    player->animation.flags |= 1;
    player->triggerAnimationCallback = PlayerCue_UpdateHidden;
}

void PlayerCue_Stand(struct Player *player, s32 facing)
{
    if (facing >= 0)
        player->facing = facing;
    Player_UpdateFacingSide(player);
    player->triggerAnimationCallback = Player_UpdateTriggerAnimation;
    Player_EnterKindTwo(player);
    ActorSequence_Begin(player, 0, 0);
}

void PlayerCue_Face(struct Player *player, u8 targetFacing, u32 parameter)
{
    player->targetFacing = targetFacing;
    player->cueParameter = parameter;
    player->triggerAnimationCallback = Player_TurnTowardTargetFacing;
    Player_EnterKindTwo(player);
    ActorSequence_Begin(player, 0, 0);
}

void PlayerCue_Idle(struct Player *player, s32 facing)
{
    if (facing >= 0)
        player->facing = facing;
    Player_UpdateFacingSide(player);
    Player_EnterKindTwo(player);
    ActorSequence_Begin(player, 0, 1);
    player->triggerAnimationCallback = Player_UpdateTriggerAnimation;
}

void PlayerCue_StandVariant2(struct Player *player, s32 facing, u32 parameter)
{
    if (facing >= 0)
        player->facing = facing;
    Player_UpdateFacingSide(player);
    player->cueParameter = parameter;
    Player_EnterKindTwo(player);
    ActorSequence_Begin(player, 0, 2);
    player->triggerAnimationCallback = Player_UpdateTriggerAnimation;
}

static inline u8 IsFrontQuadrant(u32 quadrant)
{
    if ((u8)(quadrant - 1) <= 1)
        return TRUE;
    return FALSE;
}

static inline u8 IsBackQuadrant(u32 quadrant)
{
    if (quadrant > 1)
        return TRUE;
    return FALSE;
}

void PlayerCue_StandVariant3(struct Player *player, s32 facing, u32 parameter)
{
    if (player->actionMode == 6) {
        if (player->heading > 1)
            player->heading--;
    } else {
        u8 angle = (facing << 5) + 0xA0;
        u32 quadrant = ((angle + 0x60) >> 6) & 3;
        u8 *heading = &player->heading;
        u8 *side = &player->side;
        *heading = IsFrontQuadrant(quadrant);
        *side = IsBackQuadrant(quadrant);
    }
    player->cueParameter = parameter;
    Player_EnterKindTwo(player);
    ActorSequence_Begin(player, 0, 3);
    player->triggerAnimationCallback = Player_UpdateTriggerAnimation;
}

void PlayerCue_WalkTo(struct Player *player, const struct ContactPosition *destination,
                      u32 parameter, s32 targetFacing)
{
    (*(struct ContactPosition *)&player->cueDestination) = *destination;
    player->cueParameter = parameter;
    player->targetFacing = targetFacing;
    if (player->actionMode == 3) {
        if ((s8)targetFacing >= 0)
            player->targetFacing = ((s8)targetFacing >> 1) * 2 + 1;
        player->triggerAnimationCallback = PlayerCue_UpdateWalkTarget;
    } else if (player->actionMode == 4) {
        player->triggerAnimationCallback = PlayerCue_UpdateSideWalkTarget;
    } else if (player->actionMode == 5) {
        player->actionLatch464 = 1;
        player->actionLatch465 = 1;
        player->triggerAnimationCallback = Player_AdvanceScriptDestination;
    } else {
        player->triggerAnimationCallback = Player_AdvanceScriptDestination;
    }
    Player_EnterKindTwo(player);
    ActorSequence_Begin(player, 1, 0);
}

void PlayerCue_MoveTo(struct Player *player, const struct ContactPosition *position, s32 facing,
                      s32 animated, u32 parameter)
{
    if (position == 0)
        (*(struct ContactPosition *)&player->cueDestination) =
            (*(struct ContactPosition *)&player->position);
    else
        (*(struct ContactPosition *)&player->cueDestination) = *position;
    player->cueParameter = parameter;
    player->animationBranchRequested = animated;
    player->targetFacing = facing;
    player->cuePhase = 0x96;
    Player_EnterKindTwo(player);
    ActorSequence_Begin(player, 6, 0);
    player->triggerAnimationCallback = PlayerCue_UpdateMoveDestination;
}

void PlayerCue_Motion7Front(struct Player *player, u32 variant, u32 parameter)
{
    PlayerCue_Stand(player, 4);
    player->animationBranchRequested = variant;
    player->cueParameter = parameter;
    player->facing = 4;
    player->heading = 1;
    player->side = 0;
    Player_EnterKindTwo(player);
    ActorSequence_Begin(player, 7, 0);
    player->triggerAnimationCallback = Player_UpdateSongCueAnimation;
}

void PlayerCue_Motion7(struct Player *player, u32 variant, u32 parameter)
{
    player->animationBranchRequested = variant;
    player->cueParameter = parameter;
    player->facing = 0;
    player->heading = 0;
    player->side = 0;
    Player_EnterKindTwo(player);
    ActorSequence_Begin(player, 7, 0);
    player->triggerAnimationCallback = Player_UpdateSongCueAnimation;
}

void PlayerCue_Motion7End(struct Player *player, u32 parameter)
{
    if (player->kind == 2 && player->tetherMode == 7 && player->tetherPhase != 0) {
        player->cueParameter = parameter;
        Player_EnterKindTwo(player);
        ActorSequence_Begin(player, 7, 3);
        player->triggerAnimationCallback = Player_UpdateSongCueAnimation;
    }
}

void PlayerCue_Kneel(struct Player *player, s32 facing, u32 parameter)
{
    Player_EnterKindTwo(player);
    if (facing >= 0)
        player->facing = facing;
    Player_UpdateFacingSide(player);
    player->cueParameter = parameter;
    Player_EnterKindTwo(player);
    ActorSequence_Begin(player, 0x15, 0);
    player->triggerAnimationCallback = Player_UpdateTimedCueAnimation;
}

void PlayerCue_KneelGasp(struct Player *player, s32 facing)
{
    Player_EnterKindTwo(player);
    if (facing >= 0)
        player->facing = facing;
    Player_UpdateFacingSide(player);
    Sound_PlaySong(0xC6);
    Player_EnterKindTwo(player);
    ActorSequence_Begin(player, 0x15, 2);
    player->triggerAnimationCallback = Player_UpdateTimedCueAnimation;
}

void PlayerCue_KneelDown(struct Player *player, s32 facing)
{
    Player_EnterKindTwo(player);
    if (facing >= 0)
        player->facing = facing;
    Player_UpdateFacingSide(player);
    Player_EnterKindTwo(player);
    ActorSequence_Begin(player, 0x15, 1);
    player->triggerAnimationCallback = Player_UpdateTimedCueAnimation;
}

void PlayerCue_GetUp(struct Player *player, s32 facing, u32 parameter)
{
    if (facing >= 0)
        player->facing = facing;
    Player_UpdateFacingSide(player);
    player->cueParameter = parameter;
    Player_EnterKindTwo(player);
    ActorSequence_Begin(player, 0x15, 3);
    player->triggerAnimationCallback = Player_UpdateTimedCueAnimation;
}

void PlayerCue_BeginMode27(struct Player *player, s32 facing, s32 variant, u32 parameter)
{
    if (facing >= 0)
        player->facing = facing;
    player->facing |= 1;
    player->cueParameter = parameter;
    player->animationBranchRequested = variant;
    player->cueVariant = variant;
    Player_EnterKindTwo(player);
    ActorSequence_Begin(player, 0x1B, 0);
    if (player->actionMode == 4) {
        player->triggerAnimationCallback = PlayerCue_UpdateSideDeparture;
    } else {
        if (player->actionMode == 5) {
            player->actionLatch464 = 1;
            player->actionLatch465 = 1;
        }
        player->triggerAnimationCallback = PlayerCue_UpdateDeparture;
    }
}

void PlayerCue_BeginMode28(struct Player *player, s32 facing, s32 variant, u32 parameter)
{
    if (facing >= 0)
        player->facing = facing;
    player->facing |= 1;
    player->cueParameter = parameter;
    Player_EnterKindTwo(player);
    player->terrainRecoveryCounter = variant;
    ActorSequence_Begin(player, 0x1C, 0);
    if (player->actionMode == 4) {
        player->triggerAnimationCallback = PlayerCue_UpdateSideArrival;
    } else {
        if (player->actionMode == 5) {
            player->actionLatch464 = 1;
            player->actionLatch465 = 1;
        }
        player->triggerAnimationCallback = Player_UpdateTimedDirectionAction;
    }
}

static inline void SetOddSideFacing(struct Player *player)
{
    player->facing |= 1;
    if (player->facing > 4) {
        player->heading = (8 - player->facing) >> 1;
        player->side = 1;
    } else {
        player->heading = player->facing >> 1;
        player->side = 0;
    }
}

void PlayerCue_Motion29(struct Player *player, s32 facing, u32 speed, u32 parameter)
{
    if (facing >= 0)
        player->facing = facing;
    SetOddSideFacing(player);
    player->recoilSpeed = speed;
    player->cueParameter = parameter;
    Player_EnterKindTwo(player);
    ActorSequence_Begin(player, 0x1D, 0);
    if (player->actionMode == 3)
        player->triggerAnimationCallback = PlayerCue_UpdateRecoilSlide;
    else if (player->actionMode == 4)
        player->triggerAnimationCallback = PlayerCue_UpdateSideRecoil;
    else
        player->triggerAnimationCallback = Player_UpdateRecoilAction;
}

void PlayerCue_Motion30(struct Player *player, s32 facing, u32 speed, u32 variant, u32 parameter)
{
    if (facing >= 0)
        player->facing = facing;
    SetOddSideFacing(player);
    player->recoilSpeed = speed;
    player->animationBranchRequested = variant;
    player->cueParameter = parameter;
    Player_EnterKindTwo(player);
    ActorSequence_Begin(player, 0x1E, 0);
    player->triggerAnimationCallback = PlayerCue_UpdateKnockdownRecovery;
}

void PlayerCue_Motion30End(struct Player *player, u32 parameter)
{
    if (player->kind == 2 && player->tetherMode == 0x1e && player->tetherPhase == 5) {
        player->cueParameter = parameter;
        player->tetherPhase = 4;
        Player_EnterKindTwo(player);
    }
}

void PlayerCue_Motion29B(struct Player *player, s32 facing, u32 speed, u32 parameter)
{
    if (facing >= 0)
        player->facing = facing;
    SetOddSideFacing(player);
    player->recoilSpeed = speed;
    player->cueParameter = parameter;
    Player_EnterKindTwo(player);
    ActorSequence_Begin(player, 0x1D, 0);
    player->triggerAnimationCallback = PlayerCue_UpdateRecoilRecovery;
}

void PlayerCue_BeginOddDirection(struct Player *player, s32 direction)
{
    if (direction >= 0)
        player->facing = direction;
    SetOddSideFacing(player);
    Player_EnterKindTwo(player);
    Anim_SetKeyframes(&player->animation, player->animationData,
                      (u16)(player->heading + ActorVariant_GetTableValue(player->actionMode, 0x2e)),
                      1, 2);
    Anim_Step(&player->animation, player->animationData);
    if (player->side)
        player->animation.flags |= 4;
    else
        player->animation.flags &= ~4;
    ActorSequence_Begin(player, 0x1d, 2);
    player->triggerAnimationCallback = PlayerCue_UpdateRecoilRecovery;
}

static inline u8 GetSideFacing(u8 facing)
{
    if ((u8)(facing - 2) <= 4)
        return 4;
    return 0;
}

void PlayerCue_Motion7Turn(struct Player *player, s32 facing, u32 parameter)
{
    if (facing >= 0)
        player->facing = facing;
    player->cueParameter = parameter;
    Player_EnterKindTwo(player);
    ActorSequence_Begin(player, 7, 0);
    if (player->actionMode == 3) {
        player->facing |= 1;
        if (player->facing > 4) {
            player->heading = (8 - player->facing) >> 1;
            player->side = 1;
        } else {
            player->heading = player->facing >> 1;
            player->side = 0;
        }
        player->triggerAnimationCallback = PlayerCue_UpdateMotionFade;
    } else {
        u8 *facingValue = &player->facing;
        *facingValue = GetSideFacing(*facingValue);
        Player_UpdateFacingSide(player);
        if (player->actionMode == 4) {
            player->triggerAnimationCallback = PlayerCue_UpdateSideFade;
        } else if (player->actionMode == 5) {
            player->triggerAnimationCallback = PlayerCue_UpdateAlternateFade;
        } else {
            player->heading >>= 2;
            player->side = 0;
            player->triggerAnimationCallback = PlayerCue_UpdateScaleTransition;
        }
    }
}

void PlayerCue_Motion7Variant7(struct Player *player, s32 facing, u32 parameter)
{
    if ((u8)(player->actionMode - 3) <= 2)
        PlayerCue_Motion7Turn(player, facing, parameter);
    if (facing >= 0)
        player->facing = facing;
    Player_UpdateFacingSide(player);
    player->cueParameter = parameter;
    Player_EnterKindTwo(player);
    ActorSequence_Begin(player, 7, 7);
    player->triggerAnimationCallback = PlayerCue_UpdateScaleTransition;
}

void PlayerCue_Motion7Variant3(struct Player *player, s32 facing, u32 parameter)
{
    if (facing >= 0)
        player->facing = facing;
    player->cueParameter = parameter;
    Player_EnterKindTwo(player);
    if (player->actionMode == 3) {
        player->facing |= 1;
        if (player->facing > 4) {
            player->heading = (8 - player->facing) >> 1;
            player->side = 1;
        } else {
            player->heading = player->facing >> 1;
            player->side = 0;
        }
        ActorSequence_Begin(player, 7, 3);
        player->triggerAnimationCallback = PlayerCue_UpdateMotionFade;
    } else {
        Player_UpdateFacingSide(player);
        if (player->actionMode == 4) {
            ActorSequence_Begin(player, 7, 3);
            player->triggerAnimationCallback = PlayerCue_UpdateSideFade;
        } else if (player->actionMode == 5) {
            ActorSequence_Begin(player, 7, 3);
            player->triggerAnimationCallback = PlayerCue_UpdateAlternateFade;
        } else {
            ActorSequence_Begin(player, 7, 4);
            player->triggerAnimationCallback = PlayerCue_UpdateScaleTransition;
        }
    }
}

void PlayerCue_AimDraw(struct Player *player, s32 facing, u32 parameter)
{
    if (facing >= 0)
        player->facing = facing;
    Player_UpdateFacingSide(player);
    player->cueParameter = parameter;
    Player_EnterKindTwo(player);
    ActorSequence_Begin(player, 3, 0);
    player->triggerAnimationCallback = Player_UpdateFourPhaseAction;
}

void PlayerCue_AimDrawVariant2(struct Player *player, s32 facing, u32 parameter)
{
    if (facing >= 0)
        player->facing = facing;
    Player_UpdateFacingSide(player);
    player->cueParameter = parameter;
    Player_EnterKindTwo(player);
    ActorSequence_Begin(player, 3, 2);
    player->triggerAnimationCallback = Player_UpdateFourPhaseAction;
}

void PlayerCue_AimSteady(struct Player *player, s32 facing, u32 parameter)
{
    if (facing >= 0)
        player->facing = facing;
    Player_UpdateFacingSide(player);
    player->cueParameter = parameter;
    Player_EnterKindTwo(player);
    ActorSequence_Begin(player, 3, 0);
    player->triggerAnimationCallback = PlayerCue_UpdateAimHoldA;
}

void PlayerCue_AimSteadyVariant2(struct Player *player, s32 facing, u32 parameter)
{
    if (facing >= 0)
        player->facing = facing;
    Player_UpdateFacingSide(player);
    player->cueParameter = parameter;
    Player_EnterKindTwo(player);
    ActorSequence_Begin(player, 3, 2);
    player->triggerAnimationCallback = PlayerCue_UpdateAimHoldA;
}

void PlayerCue_SetAimPose(struct Player *player, s32 facing, u32 parameter)
{
    if (facing >= 0)
        player->facing = facing;
    Player_UpdateFacingSide(player);
    player->cueParameter = parameter;
    Player_EnterKindTwo(player);
    ActorSequence_Begin(player, 3, 0);
    player->triggerAnimationCallback = PlayerCue_UpdateAimHoldB;
}

void PlayerCue_SetFiringPose(struct Player *player, s32 facing, u32 parameter)
{
    if (facing >= 0)
        player->facing = facing;
    Player_UpdateFacingSide(player);
    player->cueParameter = parameter;
    Player_EnterKindTwo(player);
    ActorSequence_Begin(player, 3, 2);
    player->triggerAnimationCallback = PlayerCue_UpdateAimHoldB;
}

void PlayerCue_Aim(struct Player *player, s32 facing, u32 parameter)
{
    if (facing >= 0)
        player->facing = facing;
    Player_UpdateFacingSide(player);
    player->cueParameter = parameter;
    Player_EnterKindTwo(player);
    ActorSequence_Begin(player, 3, 0);
    player->triggerAnimationCallback = Player_UpdateSixPhaseAction;
}

extern const s16 gUnk_086149C4[];

static inline int TruncateQ12(int x)
{
    return x >= 0 ? x >> 12 : -(-x >> 12);
}

void PlayerCue_Shoot(struct Player *player, s32 projectileParameter, u32 parameter)
{
    struct SceneVec position;
    u32 angle;
    s32 spriteId;

    position = player->position;
    angle = ((player->facing + 5) & 7) << 5;
    position.x += TruncateQ12(gUnk_086149C4[(angle + 0x40) & 0xFF] * 150);
    position.y += 0xE6;
    position.z += TruncateQ12(gUnk_086149C4[angle] * 150);
    if (player->actionMode == 6) {
        Sound_PlaySong(0x203);
        spriteId = 0x51;
    } else {
        Sound_PlaySong(0xCD);
        spriteId = 0x4B;
    }
    MovingSprite_Create_082375FC(&position, spriteId, 0, player->facing, 0x20, 0,
                                 projectileParameter);
    player->cueParameter = parameter;
    Player_EnterKindTwo(player);
    ActorSequence_Begin(player, 3, 2);
    player->triggerAnimationCallback = Player_UpdateSixPhaseAction;
}

void PlayerCue_AimHold(struct Player *player, s32 facing, u32 phase, u32 parameter)
{
    if (facing >= 0)
        player->facing = facing;
    Player_UpdateFacingSide(player);
    player->cueParameter = parameter;
    player->cuePhase = phase;
    Player_EnterKindTwo(player);
    ActorSequence_Begin(player, 3, 0);
    player->triggerAnimationCallback = PlayerCue_UpdateAim;
}

void PlayerCue_AimEnd(struct Player *player, u32 parameter)
{
    player->cueParameter = parameter;
    Player_EnterKindTwo(player);
    ActorSequence_Begin(player, 3, 5);
    player->triggerAnimationCallback = PlayerCue_UpdateAim;
}

void PlayerCue_BeginModeSeven(struct Player *player, s32 mode)
{
    if (mode == 0) {
        player->targetFacing = 1;
        Player_UpdateFacingSide(player);
        player->heading--;
        player->tetherPhase = 0;
    } else if (mode == 1) {
        player->targetFacing = 5;
        Player_UpdateFacingSide(player);
        player->heading--;
        player->tetherPhase = 0;
    } else if (mode == 2) {
        player->cuePhase = 0;
        player->tetherPhase = 5;
    } else if (mode == 3) {
        player->targetFacing = mode;
        Player_UpdateFacingSide(player);
        player->heading--;
        player->tetherPhase = 0;
    } else
        return;
    Player_EnterKindTwo(player);
    ActorSequence_Begin(player, 7, player->tetherPhase);
    player->triggerAnimationCallback = PlayerCue_UpdateTurningPalette;
}

void PlayerCue_RunTo(struct Player *player, const struct ContactPosition *destination,
                     u32 targetFacing, u32 parameter)
{
    (*(struct ContactPosition *)&player->cueDestination) = *destination;
    player->cueParameter = parameter;
    player->targetFacing = targetFacing;
    Player_EnterKindTwo(player);
    ActorSequence_Begin(player, 2, 0);
    player->triggerAnimationCallback = PlayerCue_UpdateRunTo;
}

void PlayerCue_Run(struct Player *player, s32 facing)
{
    if (facing >= 0)
        player->facing = facing;
    Player_UpdateFacingSide(player);
    Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 1), 0x20, 1);
    Player_EnterKindTwo(player);
    ActorSequence_Begin(player, 2, 0);
    player->triggerAnimationCallback = PlayerCue_UpdateRunOverlay;
}

void PlayerCue_Motion17(struct Player *player)
{
    Player_EnterKindTwo(player);
    player->facing = 1;
    player->heading = 0;
    player->side = 0;
    ActorSequence_Begin(player, 0x11, 0);
    player->triggerAnimationCallback = Player_UpdateThreePhaseBaseAnimationAction;
}

void PlayerCue_Motion17Variant2(struct Player *player)
{
    Player_EnterKindTwo(player);
    player->facing = 1;
    player->heading = 0;
    player->side = 0;
    ActorSequence_Begin(player, 0x11, 2);
    player->triggerAnimationCallback = Player_UpdateThreePhaseBaseAnimationAction;
}

void PlayerCue_Windblown(struct Player *player, s32 backSide, u32 parameter)
{
    if (backSide != 0) {
        u8 *facing = &player->facing;
        u8 zero;
        zero = 0;
        *facing = 5;
        player->heading = zero;
        player->side = 1;
    } else {
        player->facing = 3;
        player->heading = backSide;
        player->side = backSide;
    }
    player->cueParameter = parameter;
    Player_EnterKindTwo(player);
    ActorSequence_Begin(player, 0, 0);
    player->triggerAnimationCallback = Player_UpdateTransitionAnimation;
}

void PlayerCue_WindblownVariant3(struct Player *player, s32 backSide, u32 parameter)
{
    if (backSide != 0) {
        u8 *facing = &player->facing;
        u8 zero;
        zero = 0;
        *facing = 5;
        player->heading = zero;
        player->side = 1;
    } else {
        player->facing = 3;
        player->heading = backSide;
        player->side = backSide;
    }
    player->cueParameter = parameter;
    Player_EnterKindTwo(player);
    ActorSequence_Begin(player, 0, 3);
    player->triggerAnimationCallback = Player_UpdateTransitionAnimation;
}

void PlayerCue_WindblownVariant4(struct Player *player, s32 backSide, u32 parameter)
{
    if (backSide != 0) {
        u8 *facing = &player->facing;
        u8 zero;
        zero = 0;
        *facing = 5;
        player->heading = zero;
        player->side = 1;
    } else {
        player->facing = 3;
        player->heading = backSide;
        player->side = backSide;
    }
    player->cueParameter = parameter;
    Player_EnterKindTwo(player);
    ActorSequence_Begin(player, 0, 4);
    player->triggerAnimationCallback = Player_UpdateTransitionAnimation;
}

void PlayerCue_WindblownVariant2(struct Player *player, s32 backSide, u32 parameter)
{
    if (backSide != 0) {
        u8 *facing = &player->facing;
        u8 zero;
        zero = 0;
        *facing = 5;
        player->heading = zero;
        player->side = 1;
    } else {
        player->facing = 3;
        player->heading = backSide;
        player->side = backSide;
    }
    player->cueParameter = parameter;
    Player_EnterKindTwo(player);
    ActorSequence_Begin(player, 0, 2);
    player->triggerAnimationCallback = Player_UpdateTransitionAnimation;
}

void PlayerCue_StandDiagonal(struct Player *player, s32 facing, u32 parameter)
{
    if (facing >= 0)
        player->facing = facing;
    SetOddSideFacing(player);
    player->cueParameter = parameter;
    Player_EnterKindTwo(player);
    ActorSequence_Begin(player, 0, 0);
    player->triggerAnimationCallback = PlayerCue_UpdateStandDiagonal;
}

void PlayerCue_StandDiagonalB(struct Player *player, s32 facing, u32 parameter)
{
    if (facing >= 0)
        player->facing = facing;
    SetOddSideFacing(player);
    player->cueParameter = parameter;
    Player_EnterKindTwo(player);
    ActorSequence_Begin(player, 0, 0);
    player->triggerAnimationCallback = PlayerCue_UpdateStandDiagonalB;
}

/* Sets the actor's +0x3A4 mode to 5 (on) or 3 and +0x3A2/+0x3A3, then
   update PlayerCue_UpdateFaceSide. */

void PlayerCue_FaceSide(struct Player *player, u32 backSide)
{
    if (backSide) {
        u32 zero;
        u8 *facing = &player->facing;
        zero = 0;
        *facing = 5;
        player->heading = zero;
        player->side = 1;
    } else {
        player->facing = 3;
        player->heading = backSide;
        player->side = backSide;
    }
    Player_EnterKindTwo(player);
    ActorSequence_Begin(player, 0, 0);
    player->triggerAnimationCallback = PlayerCue_UpdateFaceSide;
}

s32 PlayerCue_Motion11(struct Player *player, u32 parameter)
{
    player->cueParameter = parameter;
    Player_EnterKindTwo(player);
    ActorSequence_Begin(player, 0xb, 0);
    player->triggerAnimationCallback = Player_UpdateAlternateFormCue;
    return 1;
}

s32 PlayerCue_Motion10(struct Player *player, u32 parameter)
{
    player->cueParameter = parameter;
    Player_EnterKindTwo(player);
    ActorSequence_Begin(player, 0xa, 0);
    player->triggerAnimationCallback = Player_UpdateFormChangeCue;
    return 1;
}

s32 PlayerCue_FormMotion(struct Player *player, u32 parameter)
{
    s32 started;
    player->cueParameter = parameter;
    if (player->actionMode == 0) {
        started = 0;
    } else {
        Player_EnterKindTwo(player);
        if (player->actionMode == 1) {
            ActorSequence_Begin(player, 0xa, 0);
            player->triggerAnimationCallback = PlayerCue_UpdateFormChange;
        } else if (player->actionMode == 2) {
            ActorSequence_Begin(player, 0xb, 0);
            player->triggerAnimationCallback = Player_UpdateAlternateFormCue;
        }
        started = 1;
    }
    return started;
}

u32 PlayerCue_Motion38To(struct Player *player, const struct SceneVec *destination, u32 parameter)
{
    u32 started;

    player->cueParameter = parameter;
    if (player->actionMode == 2) {
        Player_EnterKindTwo(player);
        ActorSequence_Begin(player, 0x26, 0);
        player->triggerAnimationCallback = PlayerCue_UpdateTargetDash;
        player->cueDestination.x = destination->x;
        player->cueDestination.y = destination->y;
        player->cueDestination.z = destination->z;
        started = 1;
    } else {
        started = 0;
    }
    return started;
}

u32 PlayerCue_StateMotion(struct Player *player, u32 parameter)
{
    player->cueParameter = parameter;
    if (player->actionMode <= 1 || player->actionMode == 6)
        return 0;
    if (player->actionMode == 3) {
        Player_EnterKindTwo(player);
        ActorSequence_Begin(player, 0xD, 0);
        player->triggerAnimationCallback = PlayerCue_UpdateSpiralMotion;
    } else if (player->actionMode == 4) {
        Player_EnterKindTwo(player);
        ActorSequence_Begin(player, 0xE, 0);
        player->triggerAnimationCallback = PlayerCue_UpdateSpiralRecovery;
    } else if (player->actionMode == 5) {
        player->facing = 4;
        Player_UpdateFacingSide(player);
        Player_EnterKindTwo(player);
        ActorSequence_Begin(player, 0xF, 0);
        player->triggerAnimationCallback = Player_UpdateStateMotionCue;
    } else {
        return 0;
    }
    return 1;
}

static inline int NextRandomValue(const u16 *table)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + (u32)table);
}

void PlayerCue_ShakeEffect(struct Player *player, s32 effectKind)
{
    struct SceneVec offset;

    if (player->actionMode == 3) {
        const u16 *randomTable = gRandomTable;
        offset.x = (NextRandomValue(randomTable) & 7) - 3;
        offset.y = (NextRandomValue(randomTable) & 7) - 5;
        offset.z = 0;
    } else if (player->actionMode == 4) {
        const u16 *randomTable = gRandomTable;
        offset.x = (NextRandomValue(randomTable) & 7) - 3;
        offset.y = (NextRandomValue(randomTable) & 7) - 3;
        offset.z = 0;
    } else {
        const u16 *randomTable = gRandomTable;
        offset.x = (NextRandomValue(randomTable) & 15) - 7;
        offset.y = -(NextRandomValue(randomTable) & 31);
        offset.z = 0;
    }
    if (EffectGroup_Spawn(&player->effectGroup, effectKind, 0xE10, &offset) >= 0) {
        if (player->movementModifierKind == 1)
            Sound_PlaySong(0x191);
        else if (player->movementModifierKind == 2)
            Sound_PlaySong(0x133);
        else if (player->movementModifierKind == 3)
            Sound_PlaySong(0x133);
    }
    player->scriptStatus[0] = 0;
}

void PlayerCue_Refresh(struct Player *player)
{
    Player_EmitQueuedEffects(player);
}

// confidence=low: selects player attachment descriptors and associated sound effects.

static inline void Attachment_SetValue(u16 *dest, s32 value)
{
    *dest = value;
}

static inline void Attachment_SetMode(u8 *dest, s32 value)
{
    *dest = value;
}

void Player_ConfigureAttachmentEffect(struct Player *player, u32 action, s32 value, s32 arg)
{
    switch (action) {
        case 0:
            Attachment_SetMode(&player->kindTwoAttachmentDescriptor, 0);
            Attachment_SetValue(&player->kindTwoAttachmentCounter, 0);
            player->kindTwoAttachment = ActorAttachment_UpdateFromDescriptor(
                player->kindTwoAttachment, &player->positionalEffectGroup,
                player->kindTwoAttachmentDescriptor, 1, 0, 0, 0, 80);
            Sound_PlaySong(0x1B1);
            break;

        case 1:
            Attachment_SetMode(&player->kindTwoAttachmentDescriptor, 0);
            Attachment_SetValue(&player->kindTwoAttachmentCounter, 0);
            player->kindTwoAttachment = ActorAttachment_UpdateFromDescriptor(
                player->kindTwoAttachment, &player->positionalEffectGroup,
                player->kindTwoAttachmentDescriptor, 3, 0, 0, 0, 80);
            Sound_PlaySong(0x1EC);
            break;

        case 2:
            Attachment_SetMode(&player->kindTwoAttachmentDescriptor, 0);
            Attachment_SetValue(&player->kindTwoAttachmentCounter, 0);
            player->kindTwoAttachment = ActorAttachment_UpdateFromDescriptor(
                player->kindTwoAttachment, &player->positionalEffectGroup,
                player->kindTwoAttachmentDescriptor, 0, 0, 0, 0, 80);
            Sound_PlaySong(0x1EC);
            break;

        case 3:
            Attachment_SetMode(&player->kindTwoAttachmentDescriptor, 0);
            Attachment_SetValue(&player->kindTwoAttachmentCounter, 0);
            player->kindTwoAttachment = ActorAttachment_UpdateFromDescriptor(
                player->kindTwoAttachment, &player->positionalEffectGroup,
                player->kindTwoAttachmentDescriptor, 2, 0, 0, 0, 80);
            Sound_PlaySong(0x1ED);
            break;

        case 4:
            Attachment_SetMode(&player->kindTwoAttachmentDescriptor, 1);
            Attachment_SetValue(&player->kindTwoAttachmentCounter, value);
            player->kindTwoAttachment = ActorAttachment_UpdateFromDescriptor(
                player->kindTwoAttachment, &player->positionalEffectGroup,
                player->kindTwoAttachmentDescriptor, 0, &player->kindTwoAttachmentCounter, (s16)arg,
                150, 230);
            break;

        case 5:
            Attachment_SetMode(&player->kindTwoAttachmentDescriptor, 3);
            Attachment_SetValue(&player->kindTwoAttachmentCounter, value);
            player->kindTwoAttachment = ActorAttachment_UpdateFromDescriptor(
                player->kindTwoAttachment, &player->positionalEffectGroup,
                player->kindTwoAttachmentDescriptor, 0, &player->kindTwoAttachmentCounter, (s16)arg,
                150, 230);
            break;

        case 6:
            player->kindTwoAttachmentDescriptor = 5;
            Attachment_SetValue(&player->kindTwoAttachmentCounter, 0);
            player->kindTwoAttachment = ActorAttachment_UpdateFromDescriptor(
                player->kindTwoAttachment, &player->positionalEffectGroup,
                player->kindTwoAttachmentDescriptor, 0, 0, 0, 150, 230);
            break;

        case 7:
            player->kindTwoAttachmentDescriptor = 4;
            Attachment_SetValue(&player->kindTwoAttachmentCounter, 0);
            player->kindTwoAttachment = ActorAttachment_UpdateFromDescriptor(
                player->kindTwoAttachment, &player->positionalEffectGroup,
                player->kindTwoAttachmentDescriptor, 0, 0, 0, 150, 230);
            break;

        case 8:
            Attachment_SetValue(&player->kindTwoAttachmentCounter, 0);
            player->kindTwoAttachment = ActorAttachment_UpdateFromDescriptor(
                player->kindTwoAttachment, &player->positionalEffectGroup,
                player->kindTwoAttachmentDescriptor, 127, 0, 0, 0, 80);
            break;
    }
}

void PlayerCue_ReleaseControl(struct Player *player)
{
    u32 zero;
    if (player->tetherMode != 6)
        player->tetherMode = 0;
    player->tetherPhase = 0;
    player->tetherFrame = 0;
    zero = 0;
    if (player->kindTwoEntered != 0)
        player->kindTwoEntered = zero;
    player->kind = 1;
    player->terrainClampBlocked = zero;
}
