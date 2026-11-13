#include "player_cue_actions.h"

#include "global.h"

#include "player.h"

#include "scene_vec.h"

#include "ram.h"

#include "game_state.h"

#include "player_cue_sequence.h"

#include "render.h"

#include "engine.h"

#include "contact_position.h"

#include "monster_particle_random.h"

#include "heap.h"

#include "resource_registry.h"

void PlayerCue_EnterStandUpdate(struct Player *player)
{
    ActorSequence_Begin(player, 0, 0);
    player->triggerAnimationCallback = Player_UpdateTriggerAnimation;
}

void ScriptActor_EnableCompletionScript(struct Player *player)
{
    player->kindTwoState = 1;
}

void ScriptActor_CallCompletionScript(struct Player *player)
{
    u32 *slot = &player->cueParameter;
    u32 script = *slot;
    if (script != 0) {
        *slot = 0;
        Script_Call(script, 0);
    }
}

void PlayerCue_BeginAcknowledgement(struct Player *player)
{
    player->kindTwoState = 2;
    player->cueAcknowledgementPending = 1;
}

void PlayerCue_AcknowledgePendingTrigger(struct Player *player)
{
    u8 *pending = &player->cueAcknowledgementPending;
    if (*pending != 0) {
        Trigger_ConsumePending(&player->cueReceiver, 1);
        *pending = 0;
    }
}

void PlayerCue_CompleteAction(struct Player *player)
{
    u8 kind = player->kindTwoState;
    if (kind == 1)
        ScriptActor_CallCompletionScript(player);
    else if (kind == 2)
        PlayerCue_AcknowledgePendingTrigger(player);
}

void PlayerCue_UpdateHidden(struct Player *player)
{
    Player_AddControlFlags(player, 5);
}

void PlayerCue_UpdatePeriodicParticles(struct Player *player)
{
    u8 *frame;
    s32 next;
    if (player->cueParticlesEnabled != 0) {
        frame = &player->cueParticleFrame;
        next = *frame + 1;
        *frame = next;
        if ((u8)next > 7) {
            SceneActor_SpawnModeParticles(player);
            *frame = 0;
        }
    }
}

void Player_UpdateTriggerAnimation(struct Player *player)
{
    switch ((s32)player->tetherPhase) {
        case 0:
            Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0), 0x40, 1);
            if (player->pendingAnimationTrigger) {
                player->pendingAnimationTrigger = 0;
                goto dispatch;
            }
            break;
        case 2:
            if (++player->tetherFrame > 19)
                goto reset;
        case 1:
            Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0x2F), 0x40,
                                1);
            break;
        reset:
            Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0), 0x40, 1);
            PlayerCue_EnterStandUpdate(player);
        dispatch:
            PlayerCue_CompleteAction(player);
            break;
        case 3:
            if (player->tetherFrame <= 19) {
                Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0x30),
                                    0x40, 2);
                player->tetherFrame++;
            } else {
                Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0x30),
                                    0x40, 2);
                PlayerCue_CompleteAction(player);
            }
            break;
    }
}

void Player_TurnTowardTargetFacing(struct Player *player)
{
    if (player->tetherFrame > 1) {
        s32 target = player->targetFacing;
        u8 *fp = &player->facing;
        u32 facing = *fp;
        u32 shifted = facing;
        s32 delta;
        shifted -= 8;
        target -= shifted;
        delta = target & 7;
        *fp = (facing + (delta > 4 ? 7 : 1)) & 7;
        {
            u32 current = player->facing;
            if (current > 4) {
                player->heading = 8 - current;
                player->side = 1;
            } else {
                player->heading = current;
                player->side = 0;
            }
        }
        player->tetherFrame = 0;
    } else
        player->tetherFrame++;
    Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0), 0x40, 1);
    if (player->facing == player->targetFacing) {
        PlayerCue_EnterStandUpdate(player);
        PlayerCue_CompleteAction(player);
    }
}

void Player_UpdateTransitionAnimation(struct Player *player)
{
    switch (player->tetherPhase) {
        case 0:
            if (Player_SetAnimation(player, 0x15F, 0x40, 1)) {
                player->tetherPhase = 1;
                PlayerCue_CompleteAction(player);
            }
            break;
        case 1:
            Player_SetAnimation(player, 0x160, 0x40, 1);
            break;
        case 2:
            if (Player_SetAnimation(player, 0x15F, 0x40, 1)) {
                Player_UpdateFacingSide(player);
                PlayerCue_EnterStandUpdate(player);
                PlayerCue_CompleteAction(player);
            }
            break;
        case 3:
            Player_SetAnimation(player, 0x160, 0x40, 1);
            {
                u32 frame = player->tetherFrame;
                s32 mask = 7;
                s32 low;
                /* Low three frame bits. */
                low = ~(~frame | ~mask);
                frame >>= 3;
                {
                    s32 threshold = mask - frame;
                    if (low < threshold)
                        Player_AddControlFlags(player, 5);
                }
            }
            if (++player->tetherFrame > 55) {
                ActorSequence_Begin(player, 0, 1);
                PlayerCue_CompleteAction(player);
            }
            break;
        case 4:
            Player_SetAnimation(player, 0x160, 0x40, 1);
            {
                u32 frame = player->tetherFrame;
                s32 mask = 7;
                s32 low;
                /* Low three frame bits. */
                low = ~(~frame | ~mask);
                frame >>= 3;
                {
                    s32 threshold = mask - frame;
                    if (low >= threshold)
                        Player_AddControlFlags(player, 5);
                }
            }
            if (++player->tetherFrame > 55) {
                ActorSequence_Begin(player, 0, 0);
                player->triggerAnimationCallback = PlayerCue_UpdateHidden;
                PlayerCue_CompleteAction(player);
            }
            break;
    }
}

void PlayerCue_UpdateStandDiagonal(struct Player *player)
{
    if (Player_SetAnimation(player, 0x161, 0x40, 1) != 0) {
        Player_UpdateFacingSide(player);
        PlayerCue_EnterStandUpdate(player);
        PlayerCue_CompleteAction(player);
    }
}

void PlayerCue_UpdateFaceSide(struct Player *player)
{
    s32 r;

    r = ActorVariant_GetTableValue(player->actionMode, 0x34);
    Player_SetAnimation(player, r, 0x40, 1);
    Player_AddControlFlags(player, 4);
}

// confidence=low: role inferred from callers and callback phases.

// confidence=low: approaches the scripted planar destination and returns to the standing update.

extern const s16 gUnk_086149C4[];

static inline s32 TruncateQ12(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}

static inline s32 TruncateQ6(s32 n)
{
    return n >= 0 ? n >> 6 : -((-n) >> 6);
}

static inline u32 ReadFlagBits(u32 *p, u32 mask)
{
    return *p & mask;
}

static inline u8 IsMovementSlowed(struct Player *player)
{
    s32 active = player->active;
    s32 result = 0;
    if (active && player->sceneMode == 5)
        result = 1;
    return result;
}

static inline struct Player *PlayerCue_ResolveReference(void *actor)
{
    union PlayerCueReference reference;
    reference.actor = actor;
    return reference.player;
}

void Player_AdvanceScriptDestination(struct Player *player)
{
    s32 deltaX = (player->cueDestination.x) - (player->position.x);
    s32 dz = (player->cueDestination.z) - (player->position.z);
    s32 angle = Math_Atan2(deltaX, dz);
    s32 speed = 12;
    Player_GetMovementRateScale(12);
    if (player && IsMovementSlowed(player))
        speed = (speed * player->movementSpeedScale) >> 6;
    if (((deltaX * deltaX) + (dz * dz)) <= (speed * speed)) {
        player->position.x = player->cueDestination.x;
        player->position.z = player->cueDestination.z;
        if ((player->targetFacing) >= 0) {
            player->facing = player->targetFacing;
            player->targetFacing = -1;
        } else {
            s32 shifted = (((u16)angle) + 0x70) >> 5;
            u32 mask = 7;
            mask &= shifted;
            player->facing = mask;
        }
        if (player->facing > 4) {
            player->heading = 8 - player->facing;
            player->side = 1;
        } else {
            player->heading = player->facing;
            player->side = 0;
        }
        Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0), 64, 1);
        PlayerCue_EnterStandUpdate(player);
        PlayerCue_CompleteAction(player);
    } else {
        player->velocityX = TruncateQ12(gUnk_086149C4[(angle + 64) & 255] * speed);
        player->velocityZ = TruncateQ12(gUnk_086149C4[angle & 255] * speed);
        if (player->velocityScaleAxis == 1) {
            if ((player->velocityZ) < 0)
                player->velocityZ = TruncateQ6((player->velocityZ) * 40);
            else
                player->velocityZ = TruncateQ6((player->velocityZ) * 52);
        } else if (player->velocityScaleAxis == 2) {
            if ((player->velocityX) < 0)
                player->velocityX = TruncateQ6((player->velocityX) * 40);
            else
                player->velocityX = TruncateQ6((player->velocityX) * 52);
        }
        player->facing = ((((angle + 16) & 255) >> 5) + 3) & 7;
        if (player->facing > 4) {
            player->heading = 8 - player->facing;
            player->side = 1;
        } else {
            player->heading = player->facing;
            player->side = 0;
        }
        if ((!((player->actionCueFlags) & 3)) && (!(player->actionCueCounter))) {
            if ((!((player && (player->active)) && (!(player->sceneMode)))) &&
                (!(player && ((u8)ReadFlagBits(&player->actionFlags, 4))))) {
                if (PlayerCue_ResolveReference(player) && IsMovementSlowed(player)) {
                    if (!(player->actionCueFlags))
                        Sound_PlaySong(0xC8);
                    else
                        Sound_PlaySong(0xC7);
                } else if ((player->tetherStatusFlags) || (!player->velocityScaleAxis)) {
                    if (!(player->actionCueFlags))
                        Sound_PlaySong(0xCA);
                    else
                        Sound_PlaySong(0xC9);
                } else if (!(player->actionCueFlags))
                    Sound_PlaySong(0x28E);
                else
                    Sound_PlaySong(0x28D);
            }
        }
        Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 1), 64, 1);
    }
}

// confidence=low: MoveTo callback approaches destination and decelerates its movement effect.

static inline void SetFacingFromMotionAngle(struct Player *player, u16 angle)
{
    s32 shifted = (((u16)angle) + 0x70) >> 5;
    u32 mask = 7;
    u8 direction;
    u8 *facing;
    mask &= shifted;
    facing = &player->facing;
    direction = mask;
    *facing = mask;
    if (direction > 4) {
        player->heading = 8 - direction;
        player->side = 1;
    } else {
        player->heading = mask;
        player->side = 0;
    }
}

void PlayerCue_UpdateMoveDestination(struct Player *player)
{
    s32 deltaX = (player->cueDestination.x) - (player->position.x);
    s32 dz = (player->cueDestination.z) - (player->position.z);
    s32 distance = (deltaX * deltaX) + (dz * dz);
    s32 x = 0;
    s32 z = 0;
    s32 moving;
    s32 anim;
    s32 angle;
    if (!distance) {
        anim = ActorVariant_GetTableValue(player->actionMode, 16);
        if ((player->targetFacing) >= 0) {
            player->facing = player->targetFacing;
            player->targetFacing = -1;
            {
                u32 direction = player->facing;
                if (direction > 4) {
                    player->heading = 8 - direction;
                    player->side = 1;
                } else {
                    player->heading = direction;
                    player->side = 0;
                }
            }
        }
        if (player->animationBranchRequested) {
            if (player->cuePhase) {
                moving = 1;
                angle = (((player->facing) + 1) & 7) << 5;
                x = gUnk_086149C4[(angle + 64) & 255];
                z = gUnk_086149C4[angle & 255];
                player->cuePhase = ((player->cuePhase) * 7) >> 3;
            } else {
                moving = 0;
                if (player->cueParameter) {
                    u32 script = player->cueParameter;
                    player->cueParameter = 0;
                    Script_Call(script, 0);
                }
                player->animationBranchRequested = 0;
            }
        } else {
            moving = 0;
            if (player->cueParameter) {
                u32 script = player->cueParameter;
                player->cueParameter = 0;
                Script_Call(script, 0);
            }
        }
    } else {
        anim = ActorVariant_GetTableValue(player->actionMode, 17);
        moving = 1;
        SetFacingFromMotionAngle(player, Math_Atan2(deltaX, dz));
        angle = (((player->facing) + 1) & 7) << 5;
        x = gUnk_086149C4[(angle + 64) & 255];
        z = gUnk_086149C4[angle & 255];
        if ((player->actionCueFlags) & 3) {
            if (distance <= 64) {
                player->position.x = player->cueDestination.x;
                player->position.z = player->cueDestination.z;
            } else {
                s32 speed = 8;
                player->velocityX = -TruncateQ12(x * speed);
                player->velocityZ = -TruncateQ12(z * speed);
                if ((player->velocityScaleAxis) == 1) {
                    if ((player->velocityZ) < 0)
                        player->velocityZ = TruncateQ6((player->velocityZ) * 40);
                    else
                        player->velocityZ = TruncateQ6((player->velocityZ) * 52);
                } else if ((player->velocityScaleAxis) == 2) {
                    if ((player->velocityX) < 0)
                        player->velocityX = TruncateQ6((player->velocityX) * 40);
                    else
                        player->velocityX = TruncateQ6((player->velocityX) * 52);
                }
            }
            if ((player->animationBranchRequested) && ((player->cuePhase) <= 149))
                player->cuePhase = ((player->cuePhase) * 15) >> 4;
        }
    }
    Player_SetAnimation(player, anim, 64, 1);
    if (moving)
        Player_ConstrainTetherTarget(player, x, z, player->cuePhase);
}

void Player_UpdateSongCueAnimation(struct Player *player)
{
    switch ((s32)player->tetherPhase) {
        case 0:
            player->resetValue6FC = 15;
            if (Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 8), 0x40,
                                    1)) {
                Sound_PlaySong(0xCB);
                if (player->animationBranchRequested)
                    player->tetherPhase = 1;
                else
                    player->tetherPhase = 2;
                player->animationBranchRequested = 0;
                player->tetherFrame = 0;
            }
            break;
        case 1:
            player->resetValue6FC = 30;
            Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 9), 0x40, 1);
            PlayerCue_CompleteAction(player);
            break;
        case 2:
            player->resetValue6FC = 30;
            Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 10), 0x40,
                                1);
            PlayerCue_CompleteAction(player);
            break;
        case 3:
            player->resetValue6FC = 15;
            if (Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 11),
                                    0x40, 1)) {
                u32 facing = player->facing;
                if (facing > 4) {
                    player->heading = 8 - facing;
                    player->side = 1;
                } else {
                    player->heading = facing;
                    player->side = 0;
                }
                PlayerCue_EnterStandUpdate(player);
                PlayerCue_CompleteAction(player);
            }
            break;
    }
}

// confidence=low: tentative cue role inferred from named callers and callback behavior.

// confidence=low: cue callbacks select departure and arrival phases with scale changes.

static inline void SetCueVerticalScale(struct Player *player, u32 value)
{
    player->terrainRecoveryCounter = value;
}

void PlayerCue_UpdateScaleTransition(struct Player *player)
{
    u16 *t;
    switch (player->tetherPhase) {
        case 0:
            player->resetValue6FC = 15;
            if (Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 8), 64,
                                    1))
                ActorSequence_Begin(player, 7, 1);
            break;
        case 1:
            player->resetValue6FC = 30;
            Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 10), 64, 1);
            if (++player->tetherFrame > 15)
                ActorSequence_Begin(player, 7, 2);
            break;
        case 2:
            if (player->actionMode == 6 && !player->tetherFrame)
                Sound_PlaySong(0x327);
            player->resetValue6FC = 30;
            player->animation.flags |= 2;
            t = &player->tetherFrame;
            player->movementSpeedScale = 64 - ((u8)*t << 2);
            if (*t > 8) {
                player->secondarySpeedScale = *t << 3;
                {
                    s32 frame = *t - 8;
                    SetCueVerticalScale(player, ((frame << 2) + frame) << 2);
                }
                Player_AddControlFlags(player, 4);
            }
            if (++*t > 15)
                ActorSequence_Begin(player, 7, 3);
            break;

        case 3:
            Player_AddControlFlags(player, 4);
            SetCueVerticalScale(player, player->tetherFrame * 200 + 160);
            if (++player->tetherFrame > 7) {
                Player_AddControlFlags(player, 1);
                ActorSequence_Begin(player, 0, 0);
                player->triggerAnimationCallback = PlayerCue_UpdateHidden;
                PlayerCue_CompleteAction(player);
            }
            break;
        case 4:
            if (player->actionMode == 6 && !player->tetherFrame)
                Sound_PlaySong(0x201);
            Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0), 64, 1);
            Player_AddControlFlags(player, 4);
            player->animation.flags |= 2;
            player->movementSpeedScale = 4;
            player->secondarySpeedScale = 120;
            SetCueVerticalScale(player, (8 - player->tetherFrame) * 200 + 160);
            if (++player->tetherFrame > 7)
                ActorSequence_Begin(player, 7, 5);
            break;
        case 5:
            Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0), 64, 1);
            player->animation.flags |= 2;
            player->movementSpeedScale = 64 - ((15 - (u8)player->tetherFrame) << 2);
            if (player->tetherFrame <= 7) {
                player->secondarySpeedScale = (15 - player->tetherFrame) << 3;
                SetCueVerticalScale(player, (8 - player->tetherFrame) * 20);
            } else {
                player->secondarySpeedScale = 64;
                SetCueVerticalScale(player, 0);
                Player_AddControlFlags(player, 4);
            }
            if (++player->tetherFrame > 15)
                ActorSequence_Begin(player, 7, 6);
            break;
        case 6:
            player->animation.flags &= ~2;
            Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0), 64, 1);
            if (++player->tetherFrame > 3) {
                Player_UpdateFacingSide(player);
                PlayerCue_EnterStandUpdate(player);
                PlayerCue_CompleteAction(player);
            }
            break;
        case 7:
            if (player->actionMode == 6 && !player->tetherFrame)
                Sound_PlaySong(0x327);
            Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0), 64, 1);
            player->animation.flags |= 2;
            t = &player->tetherFrame;
            player->movementSpeedScale = 64 - ((u8)*t << 2);
            if (*t > 8) {
                player->secondarySpeedScale = *t << 3;
                {
                    s32 frame = *t - 8;
                    SetCueVerticalScale(player, ((frame << 2) + frame) << 2);
                }
                Player_AddControlFlags(player, 4);
            }
            if (++*t > 15)
                ActorSequence_Begin(player, 7, 3);
            break;
        case 8:
            Player_AddControlFlags(player, 4);
            Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0), 64, 1);
            SetCueVerticalScale(player, player->tetherFrame * 200 + 160);
            if (++player->tetherFrame > 7) {
                Player_AddControlFlags(player, 1);
                ActorSequence_Begin(player, 0, 0);
                player->triggerAnimationCallback = PlayerCue_UpdateHidden;
                PlayerCue_CompleteAction(player);
            }
            break;
    }
}

void Player_UpdateTimedCueAnimation(struct Player *player)
{
    switch (player->tetherPhase) {
        case 0:
            if (!player->tetherFrame)
                Sound_PlaySong(0xD0);
            if (player->tetherFrame == 8)
                Sound_PlaySong(0xDB);
            player->tetherFrame++;
            if (Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0x31),
                                    0x40, 1)) {
                ActorSequence_Begin(player, 21, 1);
                goto dispatch;
            }
            break;
        case 1:
            Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0x32), 0x40,
                                1);
            break;
        case 3:
            if (!player->tetherFrame)
                Sound_PlaySong(0xD0);
            if (++player->tetherFrame > 4)
                goto reset;
        case 2:
            Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0x33), 0x40,
                                1);
            break;
        reset:
            Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0), 0x40, 1);
            PlayerCue_EnterStandUpdate(player);
        dispatch:
            PlayerCue_CompleteAction(player);
            break;
        case 4:
            if ((player->tetherFrame & 7) >= 7 - (player->tetherFrame >> 3))
                Player_AddControlFlags(player, 5);
            if (++player->tetherFrame > 55)
                ActorSequence_Begin(player, 21, 5);
            break;
        case 5:
            Player_AddControlFlags(player, 5);
            break;
    }
}

// confidence=low: mode 27 camera lift and scale descent precede hiding.

void PlayerCue_UpdateDeparture(struct Player *player)
{
    switch (player->tetherPhase) {
        case 0: {
            struct SceneVec pos;
            Player_AddControlFlags(player, 4);
            pos = player->position;
            pos.y += 150;
            Camera_SnapToWithoutCompletion(&pos);
            if (player->cueVariant <= 1) {
                if (player->cueVariant)
                    ScreenTransition_SendCommand(3, 3, 4, 4, 4, 0x1FFF, 16);
                else
                    ScreenTransition_SendCommand(3, 3, 4, 4, 4, 0x1FFF, 1);
            }
            if (player->facing > 4) {
                player->heading = (8 - player->facing) >> 1;
                player->side = 1;
            } else {
                player->heading = player->facing >> 1;
                player->side = 0;
            }
            Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0x29), 64,
                                1);
            Sound_PlaySong(0x1AD);
            if (player->actionMode == 6)
                Sound_PlaySong(0x2ED);
            else
                Sound_PlaySong(0x305);
            player->terrainRecoveryCounter = 0;
            ActorSequence_Begin(player, 0x1B, 1);
            break;
        }
        case 1:
            Player_AddControlFlags(player, 4);
            if (++player->tetherFrame > 5) {
                player->terrainRecoveryCounter = 0;
                player->unkA2[4] = 1;
                Sound_PlaySong(0x1AE);
                ActorSequence_Begin(player, 0x1B, 2);
            }
            break;
        case 2:
            Player_AddControlFlags(player, 4);
            Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0x12), 64,
                                1);
            player->terrainRecoveryCounter -= 60;
            if (++player->tetherFrame > 29) {
                Player_AddControlFlags(player, 1);
                player->terrainRecoveryCounter = 0;
                ActorSequence_Begin(player, 0x1B, 3);
                PlayerCue_CompleteAction(player);
            }
            break;
        case 3:
            Player_AddControlFlags(player, 5);
            break;
    }
}

void Player_UpdateTimedDirectionAction(struct Player *player)
{
    u8 phase = player->tetherPhase;
    switch (phase) {
        case 0:
            if (!player->tetherFrame) {
                if ((u8)(s8)player->facing > 4) {
                    player->heading = (8 - player->facing) >> 1;
                    player->side = 1;
                } else {
                    player->heading = (u8)(s8)player->facing >> 1;
                    player->side = phase;
                }
                player->tetherFrame++;
            }
            Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0x12), 0x40,
                                1);
            if (player->terrainRecoveryCounter > 30)
                player->terrainRecoveryCounter -= 30;
            else {
                player->terrainRecoveryCounter = 0;
                Sound_PlaySong(0x1B0);
                ActorSequence_Begin(player, 0x1C, 1);
            }
            break;
        case 1:
            if (Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0x13),
                                    0x40, 1)) {
                u8 direction = player->facing;
                if (direction > 4) {
                    player->heading = 8 - direction;
                    player->side = phase;
                } else {
                    player->heading = direction;
                    player->side = 0;
                }
                PlayerCue_EnterStandUpdate(player);
                PlayerCue_CompleteAction(player);
            }
            break;
    }
}

void Player_UpdateRecoilAction(struct Player *player)
{
    s32 value, angle;
    switch (player->tetherPhase) {
        case 0: {
            u16 frame = player->tetherFrame;
            if (!frame) {
                if ((u8)(s8)player->facing > 4) {
                    player->heading = (8 - player->facing) >> 1;
                    player->side = 1;
                } else {
                    player->heading = (u8)(s8)player->facing >> 1;
                    player->side = frame;
                }
                Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0x29),
                                    0x40, 1);
                Sound_PlaySong(0xE6);
                player->tetherFrame++;
            }
        }
            angle = ((player->facing + 5) & 7) << 5;
            value = gUnk_086149C4[(angle + 0x40) & 0xFF] * player->recoilSpeed;
            player->velocityX = -TruncateQ12(value);
            value = gUnk_086149C4[angle] * player->recoilSpeed;
            player->velocityZ = -TruncateQ12(value);
            value = (player->recoilSpeed * 3) >> 2;
            player->recoilSpeed = value;
            if (!value)
                ActorSequence_Begin(player, 0x1D, 1);
            break;
        case 1:
            if (Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0x2A),
                                    0x40, 1)) {
                Player_UpdateFacingSide(player);
                PlayerCue_EnterStandUpdate(player);
                PlayerCue_CompleteAction(player);
            }
            break;
    }
}

// confidence=low: cue motion 0x1E recoils, waits, then recovers unless requested to stay down.

void PlayerCue_UpdateKnockdownRecovery(struct Player *player)
{
    s32 value, angle;
    switch (player->tetherPhase) {
        case 0:
            Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0x29), 0x40,
                                1);
            if (++player->tetherFrame > 2)
                ActorSequence_Begin(player, 0x1E, 1);
            break;
        case 1:
            if (!player->tetherFrame)
                player->terrainRecoveryCounter = 300;
            Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0x2B), 0x40,
                                1);
            angle = ((player->facing + 5) & 7) << 5;
            value = gUnk_086149C4[(angle + 64) & 255] * player->recoilSpeed;
            player->velocityX = -TruncateQ12(value);
            value = gUnk_086149C4[angle] * player->recoilSpeed;
            player->velocityZ = -TruncateQ12(value);
            player->recoilSpeed = (player->recoilSpeed * 3) >> 2;
            if (player->tetherFrame > 3) {
                if (player->terrainRecoveryCounter >= player->tetherFrame * 8)
                    player->terrainRecoveryCounter -= player->tetherFrame * 8;
                else
                    player->terrainRecoveryCounter = 0;
            }
            player->tetherFrame++;
            if (player->recoilSpeed <= 19 && player->terrainRecoveryCounter == 0) {
                player->terrainRecoveryCounter = 0;
                Sound_PlaySong(0x2A8);
                ActorSequence_Begin(player, 0x1E, 2);
            }
            break;
        case 2:
            angle = ((player->facing + 5) & 7) << 5;
            value = gUnk_086149C4[(angle + 64) & 255] * player->recoilSpeed;
            player->velocityX = -TruncateQ12(value);
            value = gUnk_086149C4[angle] * player->recoilSpeed;
            player->velocityZ = -TruncateQ12(value);
            if (Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0x2C),
                                    0x40, 1)) {
                player->recoilSpeed = 0;
                ActorSequence_Begin(player, 0x1E, 3);
            }
            break;
        case 3:
            if (++player->tetherFrame > 19) {
                if (player->animationBranchRequested) {
                    player->animationBranchRequested = 0;
                    ActorSequence_Begin(player, 0x1E, 5);
                    PlayerCue_CompleteAction(player);
                } else
                    ActorSequence_Begin(player, 0x1E, 4);
            }
            break;
        case 4:
            if (!player->tetherFrame) {
                Sound_PlaySong(0xDB);
                player->tetherFrame++;
            }
            if (Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0x2D),
                                    0x40, 1)) {
                Sound_PlaySong(0xD0);
                Player_UpdateFacingSide(player);
                PlayerCue_EnterStandUpdate(player);
                PlayerCue_CompleteAction(player);
            }
            break;
        case 5:
            break;
    }
}

void PlayerCue_UpdateRecoilRecovery(struct Player *player)
{
    s32 value, angle;
    s32 phase = player->tetherPhase;
    switch (phase) {
        case 0: {
            u32 frame = player->tetherFrame;
            if (!frame) {
                if ((u8)(s8)player->facing > 4) {
                    player->heading = (8 - player->facing) >> 1;
                    player->side = 1;
                } else {
                    player->heading = (u8)(s8)player->facing >> 1;
                    player->side = frame;
                }
                Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0x29),
                                    0x40, 1);
                if (player->actionMode != 6)
                    Sound_PlaySong(0xE7);
            }
        }
            angle = ((player->facing + 5) & 7) << 5;
            value = gUnk_086149C4[(angle + 0x40) & 0xFF] * player->recoilSpeed;
            player->velocityX = -TruncateQ12(value);
            value = gUnk_086149C4[angle] * player->recoilSpeed;
            player->velocityZ = -TruncateQ12(value);
            value = (player->recoilSpeed * 3) >> 2;
            player->recoilSpeed = value;
            if (!value)
                ActorSequence_Begin(player, 0x1D, 1);
            break;
        case 1:
            if (player->actionCueFlags == 3 && player->actionCueCounter == 0)
                Sound_PlaySong(0xD1);
            if (Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0x2E),
                                    0x40, 1)) {
                ActorSequence_Begin(player, 0x1D, 2);
                PlayerCue_CompleteAction(player);
            }
            break;
        case 2:
            break;
    }
}

void Player_UpdateFourPhaseAction(struct Player *player)
{
    s32 speed;
    s32 result;
    if (player->actionMode == 1)
        speed = 0x30;
    else
        speed = 0x40;
    switch (player->tetherPhase) {
        case 0:
            if (Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0x15),
                                    speed, 1)) {
                ActorSequence_Begin(player, 3, 1);
                PlayerCue_CompleteAction(player);
            }
            break;
        case 1:
            Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0x16), 0x40,
                                1);
            break;
        case 2:
            if (!player->tetherFrame) {
                if (!player->actionMode)
                    Sound_PlaySong(0x38C);
                else
                    Sound_PlaySong(0x38B);
                player->tetherFrame++;
            }
            result = Player_SetAnimation(
                player, ActorVariant_GetTableValue(player->actionMode, 0x17), speed, 1);
            if (player->actionMode == 1 && player->actionCueFlags == 2 && !player->actionCueCounter)
                DirectionalSpriteEffect_LaunchFromFacing(player, player->facing, &player->position);
            if (result)
                ActorSequence_Begin(player, 3, 3);
            break;
        case 3:
            if (++player->tetherFrame > 3) {
                PlayerCue_EnterStandUpdate(player);
                PlayerCue_CompleteAction(player);
            }
            break;
    }
}

void PlayerCue_UpdateAimHoldA(void)
{
}

void PlayerCue_UpdateAimHoldB(void)
{
}

void Player_UpdateSixPhaseAction(struct Player *player)
{
    if (player->actionMode == 6)
        player->actionVariant = 9;
    else
        player->actionVariant = 8;
    switch (player->tetherPhase) {
        case 0:
            if (Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0x23),
                                    0x40, 1)) {
                Sound_PlaySong(0x1F7);
                ActorSequence_Begin(player, 3, 1);
                PlayerCue_CompleteAction(player);
            }
            break;
        case 1:
            Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0x24), 0x40,
                                1);
            break;
        case 2:
            if (Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0x24),
                                    0x40, 1))
                ActorSequence_Begin(player, 3, 3);
            break;
        case 3:
            if (Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0x25),
                                    0x40, 1))
                ActorSequence_Begin(player, 3, 4);
            break;
        case 4:
            Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0x24), 0x40,
                                1);
            if (++player->tetherFrame > 14)
                ActorSequence_Begin(player, 3, 5);
            break;
        case 5:
            if (Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0x23),
                                    0x40, 1)) {
                PlayerCue_EnterStandUpdate(player);
                PlayerCue_CompleteAction(player);
            }
            break;
    }
}

// confidence=low: tentative cue role inferred from named callers (PlayerCue_AimHold/AimEnd).

void PlayerCue_UpdateAim(struct Player *player)
{
    player->actionVariant = 4;
    switch (player->tetherPhase) {
        case 0:
            if (Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0x26),
                                    0x40, 1)) {
                Sound_PlaySong(0x1F7);
                ActorSequence_Begin(player, 3, 1);
                PlayerCue_CompleteAction(player);
            }
            break;
        case 1:
            Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0x27), 0x40,
                                1);
            player->tetherFrame++;
            if (player->tetherFrame > 5) {
                ActorSequence_Begin(player, 3, 2);
                ActorGlyphTrail_Create(player, 0x20, player->cuePhase);
            }
            break;
        case 2:
            Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0x27), 0x40,
                                1);
            if (player->tetherFrame == 6)
                Sound_PlaySong(0x277);
            player->tetherFrame++;
            if (player->tetherFrame > 0x3b) {
                Sound_PlaySong(0x278);
                ActorSequence_Begin(player, 3, 3);
            }
            break;
        case 3:
            Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0x27), 0x40,
                                1);
            player->tetherFrame++;
            if (player->tetherFrame > 7) {
                ActorSequence_Begin(player, 3, 4);
                PlayerCue_CompleteAction(player);
            }
            break;
        case 4:
            Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0x27), 0x40,
                                1);
            break;
        case 5:
            if (player->tetherFrame == 0) {
                Sound_PlaySong(0x1F6);
                player->tetherFrame++;
            }
            if (Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0x28),
                                    0x40, 1))
                ActorSequence_Begin(player, 3, 6);
            break;
        case 6:
            Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0x27), 0x40,
                                1);
            player->tetherFrame++;
            if (player->tetherFrame > 0xe)
                ActorSequence_Begin(player, 3, 7);
            break;
        case 7:
            if (Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0x26),
                                    0x40, 1)) {
                PlayerCue_EnterStandUpdate(player);
                PlayerCue_CompleteAction(player);
            }
            break;
    }
}

// confidence=low: tentative cue role inferred from named callers (PlayerCue_RunTo).

static inline s32 MultiplyMotion(s32 v, s32 k)
{
    return v * k;
}

static inline s32 ScaleDirectionalMotion(s32 v, s32 k)
{
    s32 product;
    if (k == 40)
        product = ((v << 2) + v) << 3;
    else
        product = v * k;
    if (product >= 0)
        return product >> 6;
    return -((-product) >> 6);
}

void PlayerCue_UpdateRunTo(struct Player *player)
{
    s32 deltaX;
    s32 deltaZ;
    s32 angle;
    s32 dist;
    s32 speed = 32;
    if (player->tetherFrame == 0)
        Sound_PlaySong(0x298);
    deltaX = player->cueDestination.x - player->position.x;
    deltaZ = player->cueDestination.z - player->position.z;
    angle = Math_Atan2(deltaX, deltaZ);
    dist = (deltaX * deltaX) + (deltaZ * deltaZ);
    if (dist <= 0x400) {
        player->position.x = player->cueDestination.x;
        player->position.z = player->cueDestination.z;
        if (player->targetFacing >= 0) {
            player->facing = player->targetFacing;
            player->targetFacing = -1;
        } else {
            {
                s32 t = ((u16)angle) + 0x70;
                u32 mask;
                t >>= 5;
                mask = 7;
                mask &= t;
                player->facing = mask;
            }
        }
        if (player->facing > 4) {
            player->heading = 8 - player->facing;
            player->side = 1;
        } else {
            player->heading = player->facing;
            player->side = 0;
        }
        Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0), 0x40, 1);
        PlayerCue_EnterStandUpdate(player);
        PlayerCue_CompleteAction(player);
        return;
    }
    Player_AddControlFlags(player, 4);
    if (player->tetherFrame <= 3) {
        player->actionEighteenParameter = 0x174;
        player->actionEighteenDuration = 0x18;
        Player_AddControlFlags(player, 2);
    } else if (dist <= 0x4000) {
        Player_AddControlFlags(player, 2);
    } else {
        player->actionEighteenParameter = 0x174;
        player->actionEighteenDuration = 0x18;
        Player_AddControlFlags(player, 1);
    }
    player->tetherFrame++;
    {
        s32 vy;
        player->velocityX = TruncateQ12(MultiplyMotion(gUnk_086149C4[(angle + 64) & 255], speed));
        {
            s32 sine;

            sine = gUnk_086149C4[angle & 255];

            vy = TruncateQ12(MultiplyMotion(sine, speed));
        }
        player->velocityZ = vy;
        {
            u32 axis;

            axis = player->velocityScaleAxis;

            if (axis == 1) {
                if (vy < 0)
                    player->velocityZ = ScaleDirectionalMotion(player->velocityZ, 40);
                else
                    player->velocityZ = ScaleDirectionalMotion(player->velocityZ, 0x34);
            } else if (axis == 2) {
                if (player->velocityX < 0)
                    player->velocityX = ScaleDirectionalMotion(player->velocityX, 40);
                else
                    player->velocityX = ScaleDirectionalMotion(player->velocityX, 0x34);
            }
        }
    }
    player->facing = ((((angle + 16) & 255) >> 5) + 3) & 7;
    if (player->facing > 4) {
        player->heading = 8 - player->facing;
        player->side = 1;
    } else {
        player->heading = player->facing;
        player->side = 0;
    }
    Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 1), 0x20, 1);
}

void PlayerCue_UpdateRunOverlay(struct Player *player)
{
    u16 *parameter = &player->actionEighteenParameter;
    u16 value = 0xBA;
    value <<= 1;
    *parameter = value;
    player->actionEighteenDuration = 0x18;
}

void Player_UpdateThreePhaseBaseAnimationAction(struct Player *player)
{
    switch (player->tetherPhase) {
        case 0:
            if (Player_SetAnimation(player, 0, 0x40, 1))
                ActorSequence_Begin(player, 0x11, 1);
            break;
        case 1:
            Player_SetAnimation(player, 0, 0x40, 1);
            if (!player->tetherFrame) {
                PlayerCue_CompleteAction(player);
                player->tetherFrame++;
            }
            break;
        case 2:
            if (Player_SetAnimation(player, 0, 0x40, 1)) {
                Player_UpdateFacingSide(player);
                PlayerCue_EnterStandUpdate(player);
                PlayerCue_CompleteAction(player);
            }
            break;
    }
}

// confidence=low: tentative cue role inferred from named callers and callback behavior.

// confidence=low: motion seven rotates toward a target and pulses three palette colors.

static inline u32 PackPaletteGreen(u32 otherChannels, s32 green)
{
    return (green << 5) | otherChannels;
}

void PlayerCue_UpdateTurningPalette(struct Player *player)
{
    s32 a;
    s32 b;
    s32 red;
    s32 green;
    switch (player->tetherPhase) {
        case 0:
            if (player->tetherFrame > 1) {
                if (player->targetFacing < player->facing)
                    player->facing = (player->facing - 1) & 7;
                else
                    player->facing = (player->facing + 1) & 7;
                {

                    Player_UpdateFacingSide(player);
                    player->heading--;
                    player->tetherFrame = 0;
                }
            } else
                player->tetherFrame++;
            Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0x37), 0x40,
                                1);
            if (player->facing == player->targetFacing) {
                if (player->facing == 1)
                    player->tetherPhase = 1;
                else if (player->facing == 3)
                    player->tetherPhase = 6;
                else
                    player->tetherPhase = 4;
            }
            break;

        case 1:
            player->heading = 0;
            player->side = 0;
            if (Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0x35),
                                    0x40, 1))
                player->tetherPhase = 2;
            break;

        case 2:
            player->heading = 0;
            player->side = 0;
            Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0x36), 0x40,
                                1);
            break;

        case 3:
            player->heading = 0;
            player->side = 0;
            if (Player_SetAnimation(player, 0x157, 0x40, 1)) {
                Player_UpdateFacingSide(player);
                player->heading--;
                player->tetherPhase = 0;
                player->tetherFrame = 0;
            }
            break;

        case 4:
            player->heading = 0;
            player->side = 1;
            if (Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0x38),
                                    0x40, 1))
                player->tetherPhase = 7;
            break;

        case 5:
            player->heading = 0;
            player->side = 1;
            if (Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0x39),
                                    0x40, 1)) {
                if ((++player->cuePhase) > 1)
                    player->tetherPhase = 7;
            }
            break;

        case 6:
            player->heading = 0;
            player->side = 0;
            if (Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0x38),
                                    0x40, 1))
                player->tetherPhase = 7;
            break;

        case 7:
            break;
    }

    if ((player->unk5A0) <= 7) {
        a = player->unk5A0;
        b = 8 - a;
    } else {
        a = 16 - (player->unk5A0);
        b = (player->unk5A0) - 8;
    }
    red = ((30 * a) + (19 * b)) >> 3;
    green = ((13 * a) + (12 * b)) >> 3;
    green = PackPaletteGreen(0x2C00, green) | red;
    player->visualParameterA = green;
    red = ((31 * a) + (25 * b)) >> 3;
    green = ((18 * a) + (12 * b)) >> 3;
    green = PackPaletteGreen(0x2C00, green) | red;
    player->visualParameterB = green;
    red = (31 * (a + b)) >> 3;
    green = ((22 * a) + (13 * b)) >> 3;
    green = PackPaletteGreen(0x2C00, green) | red;
    player->visualParameterC = green;
    if ((++(player->unk5A0)) > 15)
        player->unk5A0 = 0;
    if (gGameState->form != 2) {
        gPlayerCueTimer = 0;
        gPlayerActionPending = 0;
    }
    if (gPlayerActionPending) {
        if (gPlayerCueTimer)
            gPlayerCueTimer--;
        else
            gPlayerActionPending = 0;
    }
}

void Player_RestartAnimationFromSetup(struct Player *player)
{
    Anim_SetSequence(&player->cueCollider.sequence, player->cueCollider.setupResource, 0,
                     player->heading, player->side);
    PlayerCue_StepSequence((struct ContactSwarmSprite *)&player->secondaryDrawFlags,
                           &player->cueCollider.sequence);
}

// confidence=low: tentative cue role inferred from named callers and callback behavior.

void PlayerCue_UpdateWalkTarget(struct Player *player)
{
    s32 deltaX = player->cueDestination.x - player->position.x;
    s32 deltaZ = player->cueDestination.z - player->position.z;
    s32 angle = Math_Atan2(deltaX, deltaZ);
    if (deltaX * deltaX + deltaZ * deltaZ <= 81) {
        player->position.x = player->cueDestination.x;
        player->position.z = player->cueDestination.z;
        if (player->targetFacing >= 0) {
            player->facing = player->targetFacing;
            player->targetFacing = -1;
        } else {
            s32 direction = ((((angle + 16) & 255) >> 5) + 3) & 7;
            direction |= 1;
            player->facing = direction;
        }
        if (player->facing > 4) {
            player->heading = (8 - player->facing) >> 1;
            player->side = 1;
        } else {
            player->heading = player->facing >> 1;
            player->side = 0;
        }
        ActorSequence_Begin(player, 0, 0);
        player->triggerAnimationCallback = Player_RestartAnimationFromSetup;
        PlayerCue_CompleteAction(player);
    } else {
        player->velocityX = TruncateQ12(gUnk_086149C4[(angle + 64) & 255] * 9);
        player->velocityZ = TruncateQ12(gUnk_086149C4[angle & 255] * 9);
        {
            u8 d = ((((angle + 16) & 255) >> 5) + 3) & 7;
            d |= 1;
            player->facing = d;
        }
        if (player->facing > 4) {
            player->heading = (8 - player->facing) >> 1;
            player->side = 1;
        } else {
            player->heading = player->facing >> 1;
            player->side = 0;
        }
    }
    Anim_SetSequence(&player->cueCollider.sequence, player->cueCollider.setupResource, 0,
                     player->heading, player->side);
    PlayerCue_StepSequence((struct ContactSwarmSprite *)&player->secondaryDrawFlags,
                           &player->cueCollider.sequence);
}

// confidence=low: motion seven changes sprite scale and visibility over six phases.

// confidence=low: tentative player cue role; see callback body.

void PlayerCue_UpdateMotionFade(struct Player *player)
{
    switch (player->tetherPhase) {
        case 0:
            Anim_SetSequence(&player->cueCollider.sequence, player->cueCollider.setupResource, 0,
                             player->heading, player->side);
            PlayerCue_StepSequence((struct ContactSwarmSprite *)&player->secondaryDrawFlags,
                                   &player->cueCollider.sequence);
            if ((++player->tetherFrame) > 27)
                ActorSequence_Begin(player, 7, 1);
            break;

        case 1:
            Anim_SetSequence(&player->cueCollider.sequence, player->cueCollider.setupResource, 0,
                             player->heading, player->side);
            PlayerCue_StepSequence((struct ContactSwarmSprite *)&player->secondaryDrawFlags,
                                   &player->cueCollider.sequence);
            player->secondaryDrawFlags |= 2;
            player->secondaryScaleX = 64 - (((u8)player->tetherFrame) << 2);
            if (player->tetherFrame > 8) {
                player->secondaryScaleY = player->tetherFrame << 3;
                player->terrainRecoveryCounter = (player->tetherFrame - 8) * 20;
                Player_AddControlFlags(player, 4);
            }
            if ((++player->tetherFrame) > 15)
                ActorSequence_Begin(player, 7, 2);
            break;

        case 2:
            Player_AddControlFlags(player, 4);
            Anim_SetSequence(&player->cueCollider.sequence, player->cueCollider.setupResource, 0,
                             player->heading, player->side);
            PlayerCue_StepSequence((struct ContactSwarmSprite *)&player->secondaryDrawFlags,
                                   &player->cueCollider.sequence);
            player->terrainRecoveryCounter = (player->tetherFrame * 200) + 160;
            if ((++player->tetherFrame) > 7) {
                Player_AddControlFlags(player, 1);
                ActorSequence_Begin(player, 0, 0);
                player->triggerAnimationCallback = PlayerCue_UpdateHidden;
                PlayerCue_CompleteAction(player);
            }
            break;

        case 3:
            Anim_SetSequence(&player->cueCollider.sequence, player->cueCollider.setupResource, 0,
                             player->heading, player->side);
            PlayerCue_StepSequence((struct ContactSwarmSprite *)&player->secondaryDrawFlags,
                                   &player->cueCollider.sequence);
            Player_AddControlFlags(player, 4);
            player->secondaryDrawFlags |= 2;
            player->secondaryScaleX = 4;
            player->secondaryScaleY = 120;
            player->terrainRecoveryCounter = ((8 - player->tetherFrame) * 200) + 160;
            if ((++player->tetherFrame) > 7)
                ActorSequence_Begin(player, 7, 4);
            break;

        case 4:
            Anim_SetSequence(&player->cueCollider.sequence, player->cueCollider.setupResource, 0,
                             player->heading, player->side);
            PlayerCue_StepSequence((struct ContactSwarmSprite *)&player->secondaryDrawFlags,
                                   &player->cueCollider.sequence);
            player->secondaryDrawFlags |= 2;
            player->secondaryScaleX = 64 - ((15 - ((u8)player->tetherFrame)) << 2);
            if (player->tetherFrame <= 7) {
                player->secondaryScaleY = (15 - player->tetherFrame) << 3;
                player->terrainRecoveryCounter = (8 - player->tetherFrame) * 20;
            } else {
                player->secondaryScaleY = 64;
                player->terrainRecoveryCounter = 0;
                Player_AddControlFlags(player, 4);
            }
            if ((++player->tetherFrame) > 15)
                ActorSequence_Begin(player, 7, 5);
            break;

        case 5:
            player->secondaryDrawFlags &= ~2;
            Anim_SetSequence(&player->cueCollider.sequence, player->cueCollider.setupResource, 0,
                             player->heading, player->side);
            PlayerCue_StepSequence((struct ContactSwarmSprite *)&player->secondaryDrawFlags,
                                   &player->cueCollider.sequence);
            if ((++player->tetherFrame) > 3) {
                PlayerCue_EnterStandUpdate(player);
                player->triggerAnimationCallback = Player_RestartAnimationFromSetup;
                PlayerCue_CompleteAction(player);
            }
            break;
    }
}

// confidence=low: tentative cue role inferred from named callers and callback behavior.

// confidence=low: motion 0x1D applies decaying recoil until standing resumes.

void PlayerCue_UpdateRecoilSlide(struct Player *player)
{
    s32 angle;
    s32 value;
    if (!player->tetherFrame) {
        Sound_PlaySong(0xE6);
        player->tetherFrame++;
    }
    Anim_SetSequence(&player->cueCollider.sequence, player->cueCollider.setupResource, 1,
                     player->heading, player->side);
    PlayerCue_StepSequence((struct ContactSwarmSprite *)&player->secondaryDrawFlags,
                           &player->cueCollider.sequence);
    if (player->recoilSpeed) {
        angle = ((player->facing + 5) & 7) << 5;
        value = gUnk_086149C4[(angle + 64) & 255] * player->recoilSpeed;
        player->velocityX = -TruncateQ12(value);
        value = gUnk_086149C4[angle] * player->recoilSpeed;
        player->velocityZ = -TruncateQ12(value);
        player->recoilSpeed = (player->recoilSpeed * 3) >> 2;
    } else {
        player->recoilSpeed = 0;
        ActorSequence_Begin(player, 0, 0);
        player->triggerAnimationCallback = Player_RestartAnimationFromSetup;
        PlayerCue_CompleteAction(player);
    }
}

void PlayerCue_UpdateAlternateFormChange(struct Player *player)
{
    if (Player_ConsumeStateEntry(player)) {
        player->statusValue = 4000;
        player->actionEighteenParameter = 0x16E;
        player->actionEighteenDuration = 0;
        player->actionEighteenEnabled = 1;
        Sound_PlaySong(0x306);
        player->auxiliarySprite[0].sprite.flags &= ~1;
        player->auxiliarySprite[0].sprite.position.x = (u16)gGameState->positionX - 6;
        player->auxiliarySprite[0].sprite.position.y = 450 + (u16)gGameState->positionY;
        player->auxiliarySprite[0].sprite.position.z = (u16)gGameState->positionZ + 6;
        Anim_SetSequence(&player->auxiliarySequence, player->auxiliaryResource, 21, 0, 0);
        player->auxiliarySequenceEnded = 0;
    }
    if (player->auxiliarySequenceEnded)
        Anim_SetSequence(&player->auxiliarySequence, player->auxiliaryResource, 21, 0, 0);
    player->auxiliarySequenceEnded = PlayerCue_StepSequenceEnded(
        &player->auxiliarySprite[0].sprite, (void *)(&player->auxiliarySequence));
    if (player->auxiliarySequence.unknownA == 21) {
        if (player->auxiliarySprite[0].sprite.position.pad == 1)
            player->auxiliarySprite[0].sprite.position.y -= 10;
        else
            player->auxiliarySprite[0].sprite.position.y += 10;
    }
    if ((player->auxiliarySprite[0].sprite.position.pad == 1) &&
        (((s16)player->auxiliarySprite[0].sprite.position.y) < (gGameState->positionY + 220)))
        player->auxiliarySprite[0].sprite.position.y = (u16)gGameState->positionY + 220;
    switch (player->tetherPhase) {
        case 0:
            if (((u8)(player->facing - 2)) <= 4) {
                player->facing = 4;
                player->heading = 4;
                player->side = 0;
            } else {
                player->facing = 0;
                player->heading = 0;
                player->side = 0;
            }
            Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0), 64, 1);
            player->heading >>= 2;
            if (player->actionEighteenEnabled) {
                struct ContactPackedVector particles;
                particles.x = 0;
                particles.y = 0;
                particles.z = 15;
                SpiralTrail_CreateWithParticles(player, 0, &particles, 50, 50);
                Sound_PlaySong(0x39B);
            }
            Player_SetActionPhase(player, 1);
            break;

        case 1:
            if (player->actionEighteenEnabled) {
                if ((player->actionEighteenDuration += 3) > 32)
                    player->actionEighteenDuration = 32;
            }
            player->resetValue6FC = 15;
            if (Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 8), 64,
                                    1)) {
                if (player->actionEighteenEnabled)
                    Player_SetActionPhase(player, 2);
                else
                    Player_SetActionPhase(player, 3);
            }
            break;

        case 2: {
            if ((player->tetherFrame & 7) < (7 - (player->tetherFrame >> 2)))
                Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 10), 64,
                                    1);
            else if (player->actionMode == 0)
                Player_SetAnimation(player, ActorVariant_GetTableValue(2, 10), 64, 1);
            else
                Player_SetAnimation(player, ActorVariant_GetTableValue(0, 10), 64, 1);
            if ((player->actionEighteenDuration += 3) > 32)
                player->actionEighteenDuration = 32;
            player->resetValue6FC = 30;
            if ((++player->tetherFrame) > 9) {
                if (!player->actionMode)
                    player->actionMode = 2;
                else
                    player->actionMode = 0;
                Player_InitVisualResources(player);
                player->savedSpriteResource = 0xFFFF;
                if ((player->unk536 != 21) && (player->unk536 != 49))
                    player->active = 0;
                Player_SetActionPhase(player, 4);
                player->auxiliarySprite[0].sprite.flags |= 1;
            }
            break;
        }

        case 3:
            if (!player->tetherFrame)
                Sound_PlaySong(0x192);
            Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 10), 64, 1);
            player->resetValue6FC = 30;
            if ((++player->tetherFrame) > 15)
                Player_SetActionPhase(player, 4);
            break;

        case 4:
            player->resetValue6FC = 15;
            ++player->tetherFrame;
            if (player->actionMode != 2) {
                if (Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 11),
                                        64, 1)) {
                    Player_UpdateFacingSide(player);
                    ActorSequence_Begin(player, 0, 0);
                    break;
                }
            } else {
                player->formCueParameterA = 0xFFF4;
                player->formCueParameterB = 12;
                Anim_SetKeyframes(&player->animation, player->animationData, 0x1AB, 1, 0);
                Anim_Step(&player->animation, player->animationData);
            }
            Player_InitVisualResources(player);
            player->savedSpriteResource = 0xFFFF;
            player->actionEighteenDuration = 0;
            if ((++player->tetherFrame) > 29) {
                Player_UpdateFacingSide(player);
                player->actionLatch464 = 1;
                player->actionLatch465 = 2;
                Player_UpdateFacingSide(player);
                PlayerCue_EnterStandUpdate(player);
                player->pendingAnimationTrigger = 1;
            }
            break;
    }
}

// confidence=low: tentative player cue field interpretations.

void PlayerCue_UpdateFormChange(struct Player *player)
{
    if (Player_ConsumeStateEntry(player)) {
        player->actionEighteenParameter = 0x16F;
        player->actionEighteenDuration = 0;
        player->actionEighteenEnabled = 1;
        Sound_PlaySong(0x306);
        if (player->actionMode != 1) {
            player->unk432[0] = 1;
            player->unk432[1] = 1;
        } else {
            player->unk432[0] = 0;
            player->unk432[1] = 1;
        }
    }
    player->statusValue = 4000;
    Player_AddControlFlags(player, 8);
    switch (player->tetherPhase) {
        case 0:
            if (((u8)(player->facing - 2)) <= 4) {
                player->facing = 4;
                player->heading = 4;
                player->side = 0;
            } else {
                player->facing = 0;
                player->heading = 0;
                player->side = 0;
            }
            Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0), 64, 1);
            player->heading >>= 2;
            if (player->actionEighteenEnabled) {
                struct ContactPosition offset;
                offset.x = 0;
                offset.y = 0;
                offset.z = 15;
                SpiralTrail_CreateWithParticles(player, 1, &offset, 50, 69);
                Sound_PlaySong(0x39B);
            }
            Player_SetActionPhase(player, 1);
            break;

        case 1:
            if (player->actionEighteenEnabled) {
                if ((player->actionEighteenDuration += 3) > 32)
                    player->actionEighteenDuration = 32;
            }
            player->resetValue6FC = 15;
            if (Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 8), 64,
                                    1)) {
                Player_SetActionPhase(player, 2);
                player->statHundredths = 0;
                player->statusValue = 4000;
            }
            break;

        case 2:
            if ((player->tetherFrame & 7) < (7 - (player->tetherFrame >> 2)))
                Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 10), 64,
                                    1);
            else if (player->actionMode != 1) {
                Anim_SetKeyframes(&player->animation,
                                  (struct DisplaySequenceState *)player->animationData, 0xE4, 0, 0);
                Anim_Step(&player->animation, (struct DisplaySequenceState *)player->animationData);
            } else
                Player_SetAnimation(player, ActorVariant_GetTableValue(0, 10), 64, 1);
            if ((player->actionEighteenDuration += 3) > 32)
                player->actionEighteenDuration = 32;
            player->resetValue6FC = 30;
            if ((++player->tetherFrame) > 19) {
                if (player->actionMode != 1)
                    player->actionMode = 1;
                else
                    player->actionMode = 0;
                Player_InitVisualResources(player);
                player->savedSpriteResource = 65535;
                if ((player->unk536 != 21) && (player->unk536 != 49))
                    player->active = 0;
                Player_SetActionPhase(player, 3);
            }
            break;

        case 3:
            player->resetValue6FC = 15;
            if (player->actionMode != 1) {
                if (Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 11),
                                        64, 1)) {
                    Player_UpdateFacingSide(player);
                    ActorSequence_Begin(player, 0, 0);
                    Player_UpdateFacingSide(player);
                    player->actionLatch464 = 1;
                    player->actionLatch465 = 0;
                    Player_UpdateFacingSide(player);
                    PlayerCue_EnterStandUpdate(player);
                    player->pendingAnimationTrigger = 1;
                }
            } else {
                if (player->tetherFrame == 0)
                    Anim_SetKeyframes(&player->animation,
                                      (struct DisplaySequenceState *)player->animationData, 0xE4, 1,
                                      0);
                Anim_Step(&player->animation, (struct DisplaySequenceState *)player->animationData);
                if ((++player->tetherFrame) > 29) {
                    Player_UpdateFacingSide(player);
                    ActorSequence_Begin(player, 0, 0);
                    Player_UpdateFacingSide(player);
                    player->actionLatch464 = 1;
                    player->actionLatch465 = 1;
                    Player_UpdateFacingSide(player);
                    PlayerCue_EnterStandUpdate(player);
                    player->pendingAnimationTrigger = 1;
                }
            }
            break;
    }
}

// confidence=low: tentative player cue role; see callback body.

static inline void PlayerCue_AdvanceSpiralCounters(struct Player *player, u16 trailLimit,
                                                   u16 timerLimit)
{
    if ((player->actionEighteenDuration += 3) > trailLimit)
        player->actionEighteenDuration = trailLimit;
    if (++player->tetherFrame > timerLimit) {
        player->actionLatch464 = 1;
        player->actionLatch465 = 1;
        Player_UpdateFacingSide(player);
        PlayerCue_EnterStandUpdate(player);
        player->pendingAnimationTrigger = 1;
    }
}

void PlayerCue_UpdateSpiralMotion(struct Player *player)
{
    if (player->tetherFrame == 0) {
        if (player->side)
            player->facing = 7 - (player->heading * 2);
        else
            player->facing = (player->heading * 2) + 1;
        player->actionEighteenDuration = 0;
        Sound_PlaySong(0x37F);
        SpiralTrail_CreateWithParticles(player, 1, 0, 32, 56);
    }
    if ((player->tetherFrame & 7) < (7 - (player->tetherFrame >> 2))) {
        if (player->facing > 3) {
            player->heading = (7 - player->facing) >> 1;
            player->side = 1;
        } else {
            player->heading = player->facing >> 1;
            player->side = 0;
        }
    } else {
        Player_AddControlFlags(player, 0x41);
        player->previousPosition = player->position;
        if (player->facing > 4) {
            player->heading = 8 - player->facing;
            player->side = 1;
        } else {
            player->heading = player->facing;
            player->side = 0;
        }
    }
    Player_SetAnimation(player, 0x9A, 64, 1);
    {
        u16 *destination = &player->spriteResource;
        u32 resource = 0x16F;
        *destination = resource;
        player->actionVariantResource = (u32)(gPaletteBankData + (resource << 5));
    }
    Anim_SetSequence(&player->cueCollider.sequence, (void *)player->cueCollider.setupResource, 0,
                     player->heading, player->side);
    PlayerCue_StepSequence((struct ContactSwarmSprite *)&player->secondaryDrawFlags,
                           &player->cueCollider.sequence);
    {
        u16 *destination = &player->actionEighteenParameter;
        u32 resource = 0x2DD;
        *destination = resource;
    }
    PlayerCue_AdvanceSpiralCounters(player, 32, 31);
}

void Player_RestartAnimationFromResource(struct Player *player)
{
    Anim_SetSequence(&player->cueCollider.sequence, player->cueCollider.sideResource, 0,
                     player->heading, player->side);
    PlayerCue_StepSequence((struct ContactSwarmSprite *)&player->secondaryDrawFlags,
                           &player->cueCollider.sequence);
}

// confidence=low: tentative player cue role; see callback body.

static inline void PlayerCue_FaceSideWalkAngle(struct Player *player, u16 angle)
{
    s32 shifted = (angle + 112) >> 5;
    u32 mask = 7;
    u8 direction;
    u8 *facing;
    mask &= shifted;
    facing = &player->facing;
    direction = mask;
    *facing = mask;
    if (direction > 4) {
        player->heading = 8 - direction;
        player->side = 1;
    } else {
        u8 *heading = &player->heading;
        *heading = mask;
        player->side = 0;
    }
}

// confidence=low: state-four variant uses eight-facing animation and axis-dependent speed.

void PlayerCue_UpdateSideWalkTarget(struct Player *player)
{
    s32 deltaX = player->cueDestination.x - player->position.x,
        deltaZ = player->cueDestination.z - player->position.z;
    s32 angle = Math_Atan2(deltaX, deltaZ);
    if (deltaX * deltaX + deltaZ * deltaZ <= 144) {
        player->position.x = player->cueDestination.x;
        player->position.z = player->cueDestination.z;
        if (player->targetFacing >= 0) {
            player->facing = player->targetFacing;
            player->targetFacing = -1;
        } else {
            s32 shifted = ((u16)angle + 112) >> 5;
            u32 mask = 7;
            mask &= shifted;
            player->facing = mask;
        }
        if (player->facing > 4) {
            player->heading = 8 - player->facing;
            player->side = 1;
        } else {
            player->heading = player->facing;
            player->side = 0;
        }
        Anim_SetSequence(&player->cueCollider.sequence, player->cueCollider.sideResource, 0,
                         player->heading, player->side);
        PlayerCue_StepSequence((struct ContactSwarmSprite *)&player->secondaryDrawFlags,
                               &player->cueCollider.sequence);
        ActorSequence_Begin(player, 0, 0);
        player->triggerAnimationCallback = Player_RestartAnimationFromResource;
        PlayerCue_CompleteAction(player);
    } else {
        player->velocityX = TruncateQ12(gUnk_086149C4[(angle + 64) & 255] * 12);
        player->velocityZ = TruncateQ12(gUnk_086149C4[angle & 255] * 12);
        if (player->velocityScaleAxis == 1) {
            s32 v;
            if (player->velocityZ < 0) {
                v = player->velocityZ * 40;
                player->velocityZ = TruncateQ6(v);
            } else {
                v = player->velocityZ * 52;
                player->velocityZ = TruncateQ6(v);
            }
        } else if (player->velocityScaleAxis == 2) {
            s32 v;
            if (player->velocityX < 0) {
                v = player->velocityX * 40;
                player->velocityX = TruncateQ6(v);
            } else {
                v = player->velocityX * 52;
                player->velocityX = TruncateQ6(v);
            }
        }
        PlayerCue_FaceSideWalkAngle(player, angle);
        Anim_SetSequence(&player->cueCollider.sequence, player->cueCollider.sideResource, 1,
                         player->heading, player->side);
        PlayerCue_StepSequence((struct ContactSwarmSprite *)&player->secondaryDrawFlags,
                               &player->cueCollider.sequence);
    }
}

// confidence=low: tentative player cue role; see callback body.

static inline void FoldSideFacingZero(struct Player *player)
{
    if (player->facing > 4) {
        player->heading = (8 - player->facing) >> 1;
        player->side = 1;
    } else {
        player->heading = player->facing >> 1;
        player->side = 0;
    }
}

static inline void FoldSideFacing(struct Player *player, u32 phase)
{
    if (player->facing > 4) {
        player->heading = (8 - player->facing) >> 1;
        player->side = 1;
    } else {
        player->heading = player->facing >> 1;
        player->side = phase;
    }
}

// confidence=low: camera lift and scale descent precede hidden cue state.

void PlayerCue_UpdateSideDeparture(struct Player *player)
{
    switch (player->tetherPhase) {
        case 0: {
            struct SceneVec pos;
            Player_AddControlFlags(player, 4);
            pos = player->position;
            pos.y += 150;
            Camera_SnapToWithoutCompletion(&pos);
            if (player->cueVariant <= 1) {
                if (player->cueVariant)
                    ScreenTransition_SendCommand(3, 3, 4, 4, 4, 0x1FFF, 16);
                else
                    ScreenTransition_SendCommand(3, 3, 4, 4, 4, 0x1FFF, 1);
            }
            FoldSideFacingZero(player);
            Anim_SetSequence(&player->cueCollider.sequence, player->cueCollider.sideResource, 2,
                             player->heading, player->side);
            PlayerCue_StepSequence((struct ContactSwarmSprite *)&player->secondaryDrawFlags,
                                   &player->cueCollider.sequence);
            Sound_PlaySong(0x1AD);
            ActorSequence_Begin(player, 0x1B, 1);
            player->terrainRecoveryCounter = 0;
            break;
        }
        case 1:
            Player_AddControlFlags(player, 4);
            if (++player->tetherFrame > 5) {
                player->terrainRecoveryCounter = 0;
                player->secondaryPriority = 1;
                Sound_PlaySong(0x1AE);
                ActorSequence_Begin(player, 0x1B, 2);
            }
            break;
        case 2:
            Player_AddControlFlags(player, 4);
            player->terrainRecoveryCounter -= 60;
            if (++player->tetherFrame > 29) {
                Player_AddControlFlags(player, 1);
                ActorSequence_Begin(player, 0x1B, 3);
                PlayerCue_CompleteAction(player);
            }
            break;
        case 3:
            Player_AddControlFlags(player, 5);
            break;
    }
}

// confidence=low: tentative cue role from callback phases.

// confidence=low: reduces arrival scale and returns to standing after animation.

void PlayerCue_UpdateSideArrival(struct Player *player)
{
    u32 phase = player->tetherPhase;
    switch ((s16)phase) {
        case 0:
            if (!player->tetherFrame) {
                FoldSideFacing(player, phase);
                ++player->tetherFrame;
            }
            Anim_SetSequence(&player->cueCollider.sequence, player->cueCollider.sideResource, 2,
                             player->heading, player->side);
            PlayerCue_StepSequence((struct ContactSwarmSprite *)&player->secondaryDrawFlags,
                                   &player->cueCollider.sequence);
            if (player->terrainRecoveryCounter > 30)
                player->terrainRecoveryCounter -= 30;
            else {
                player->terrainRecoveryCounter = 0;
                Sound_PlaySong(0x1B0);
                ActorSequence_Begin(player, 0x1C, 1);
            }
            break;
        case 1:
            Anim_SetSequence(&player->cueCollider.sequence, player->cueCollider.sideResource, 3,
                             player->heading, player->side);
            if (PlayerCue_StepSequenceEndedWithFirstFlipMask(
                    (struct ContactSwarmSprite *)&player->secondaryDrawFlags,
                    &player->cueCollider.sequence, phase)) {
                Player_UpdateFacingSide(player);
                ActorSequence_Begin(player, 0, 0);
                player->triggerAnimationCallback = Player_RestartAnimationFromResource;
                PlayerCue_CompleteAction(player);
            }
            break;
    }
}

// confidence=low: tentative cue role from callback phases.

void PlayerCue_UpdateSideFade(struct Player *player)
{
    switch (player->tetherPhase) {
        case 0:
            if (!player->tetherFrame) {
                player->facing = 4;
                Player_UpdateFacingSide(player);
            }
            Anim_SetSequence(&player->cueCollider.sequence, player->cueCollider.sideResource, 0,
                             player->heading, player->side);
            PlayerCue_StepSequence((struct ContactSwarmSprite *)&player->secondaryDrawFlags,
                                   &player->cueCollider.sequence);
            if ((++player->tetherFrame) > 27)
                ActorSequence_Begin(player, 7, 1);
            break;

        case 1:
            player->secondaryDrawFlags |= 2;
            player->secondaryScaleX = 64 - (((u8)player->tetherFrame) << 2);
            if (player->tetherFrame > 8) {
                player->secondaryScaleY = player->tetherFrame << 3;
                player->terrainRecoveryCounter = (player->tetherFrame - 8) * 20;
                Player_AddControlFlags(player, 4);
            }
            if ((++player->tetherFrame) > 15)
                ActorSequence_Begin(player, 7, 2);
            break;

        case 2:
            Player_AddControlFlags(player, 4);
            player->terrainRecoveryCounter = (player->tetherFrame * 200) + 160;
            if ((++player->tetherFrame) > 7) {
                Player_AddControlFlags(player, 1);
                ActorSequence_Begin(player, 0, 0);
                player->triggerAnimationCallback = PlayerCue_UpdateHidden;
                PlayerCue_CompleteAction(player);
            }
            break;

        case 3:
            if (!player->tetherFrame) {
                player->facing = 4;
                Player_UpdateFacingSide(player);
            }
            Anim_SetSequence(&player->cueCollider.sequence, player->cueCollider.sideResource, 0,
                             player->heading, player->side);
            PlayerCue_StepSequence((struct ContactSwarmSprite *)&player->secondaryDrawFlags,
                                   &player->cueCollider.sequence);
            Player_AddControlFlags(player, 4);
            player->secondaryDrawFlags |= 2;
            player->secondaryScaleX = 4;
            player->secondaryScaleY = 120;
            player->terrainRecoveryCounter = ((8 - player->tetherFrame) * 200) + 160;
            if ((++player->tetherFrame) > 7)
                ActorSequence_Begin(player, 7, 4);
            break;

        case 4:
            Anim_SetSequence(&player->cueCollider.sequence, player->cueCollider.sideResource, 0,
                             player->heading, player->side);
            PlayerCue_StepSequence((struct ContactSwarmSprite *)&player->secondaryDrawFlags,
                                   &player->cueCollider.sequence);
            player->secondaryDrawFlags |= 2;
            player->secondaryScaleX = 64 - ((15 - ((u8)player->tetherFrame)) << 2);
            if (player->tetherFrame <= 7) {
                player->secondaryScaleY = (15 - player->tetherFrame) << 3;
                player->terrainRecoveryCounter = (8 - player->tetherFrame) * 20;
            } else {
                player->secondaryScaleY = 64;
                player->terrainRecoveryCounter = 0;
                Player_AddControlFlags(player, 4);
            }
            if ((++player->tetherFrame) > 15)
                ActorSequence_Begin(player, 7, 5);
            break;

        case 5:
            player->secondaryDrawFlags &= ~2;
            Anim_SetSequence(&player->cueCollider.sequence, player->cueCollider.sideResource, 0,
                             player->heading, player->side);
            PlayerCue_StepSequence((struct ContactSwarmSprite *)&player->secondaryDrawFlags,
                                   &player->cueCollider.sequence);
            if ((++player->tetherFrame) > 3) {
                Player_UpdateFacingSide(player);
                PlayerCue_EnterStandUpdate(player);
                player->triggerAnimationCallback = Player_RestartAnimationFromResource;
                PlayerCue_CompleteAction(player);
            }
            break;
    }
}

// confidence=low: tentative cue role from callback phases.

// confidence=low: inferred from the side-facing cue callbacks and effects.

void PlayerCue_UpdateSideRecoil(struct Player *player)
{
    if (!player->tetherFrame) {
        Sound_PlaySong(0xE6);
        ++player->tetherFrame;
    }
    Anim_SetSequence(&player->cueCollider.sequence, player->cueCollider.sideResource, 2,
                     player->heading, player->side);
    PlayerCue_StepSequence((struct ContactSwarmSprite *)&player->secondaryDrawFlags,
                           &player->cueCollider.sequence);
    if (player->recoilSpeed) {
        u8 *direction;
        u32 angle;
        direction = &player->facing;
        angle = ((*direction + 5) & 7) << 5;
        player->velocityX = -TruncateQ12(gUnk_086149C4[(angle + 64) & 255] * player->recoilSpeed);
        player->velocityZ = -TruncateQ12(gUnk_086149C4[angle] * player->recoilSpeed);
        player->recoilSpeed = (player->recoilSpeed * 3) >> 2;
    } else {
        player->recoilSpeed = 0;
        Player_UpdateFacingSide(player);
        ActorSequence_Begin(player, 0, 0);
        player->triggerAnimationCallback = Player_RestartAnimationFromResource;
        PlayerCue_CompleteAction(player);
    }
}

// confidence=low: inferred from player cue phases and animation consumers.

// confidence=low: tentative cue role from callback phases.

static inline void PlayerCue_AdvanceRecoveryCounters(struct Player *player, u16 trailLimit,
                                                     u16 timerLimit)
{
    if ((player->actionEighteenDuration += 3) > trailLimit)
        player->actionEighteenDuration = trailLimit;
    if (++player->tetherFrame > timerLimit) {
        player->actionLatch464 = 1;
        player->actionLatch465 = 1;
        PlayerCue_EnterStandUpdate(player);
        player->pendingAnimationTrigger = 1;
    }
}

// confidence=low: inferred from the side-facing cue callbacks and effects.

void PlayerCue_UpdateSpiralRecovery(struct Player *player)
{
    if (!player->tetherFrame) {
        player->actionEighteenDuration = 0;
        Sound_PlaySong(0x37F);
        SpiralTrail_CreateWithParticles(player, 1, 0, 32, 56);
    }
    if ((player->tetherFrame & 7) >= 7 - (player->tetherFrame >> 2)) {
        Player_AddControlFlags(player, 0x41);
        (*(struct ContactPosition *)&player->previousPosition) =
            (*(struct ContactPosition *)&player->position);
    }
    Player_SetAnimation(player, 0x9A, 0x40, 1);
    {
        u16 *target = &player->spriteResource;
        u32 resource;
        resource = 0x16F;
        *target = resource;
        player->actionVariantResource = (u32)(gPaletteBankData + (resource << 5));
    }
    {
        struct DisplaySequenceState *seq;
        seq = &player->cueCollider.sequence;
        Anim_SetSequence(seq, player->cueCollider.sideResource, 0, player->heading, player->side);
        {
            PlayerCue_StepSequence((struct ContactSwarmSprite *)&player->secondaryDrawFlags, seq);
        }
    }
    {
        u16 *target;
        u32 resource;
        target = &player->actionEighteenParameter;
        resource = 0x16F;
        *target = resource;
    }
    PlayerCue_AdvanceRecoveryCounters(player, 32, 31);
}

void PlayerCue_RestartAlternateAnimation(struct Player *player)
{
    Anim_SetSequence(&player->cueCollider.sequence, player->cueCollider.alternateResource,
                     player->movementMode46B, 0, 0);
    PlayerCue_StepSequence((struct ContactSwarmSprite *)&player->secondaryDrawFlags,
                           &player->cueCollider.sequence);
}

// confidence=low: replays the alternate cue resource after fade completion.

// confidence=low: inferred from player cue phases and animation consumers.

// confidence=low: tentative cue role from callback phases.

// confidence=low: motion 7 changes sprite scale and visibility over six phases.

void PlayerCue_UpdateAlternateFade(struct Player *player)
{
    switch (player->tetherPhase) {
        case 0:
            Anim_SetSequence(&player->cueCollider.sequence, player->cueCollider.alternateResource,
                             player->movementMode46B, 0, 0);
            PlayerCue_StepSequence((struct ContactSwarmSprite *)&player->secondaryDrawFlags,
                                   &player->cueCollider.sequence);
            if ((++player->tetherFrame) > 27)
                ActorSequence_Begin(player, 7, 1);
            break;

        case 1:
            player->secondaryDrawFlags |= 2;
            player->secondaryScaleX = 64 - (((u8)player->tetherFrame) << 2);
            if (player->tetherFrame > 8) {
                player->secondaryScaleY = player->tetherFrame << 3;
                player->terrainRecoveryCounter = (player->tetherFrame - 8) * 20;
                Player_AddControlFlags(player, 4);
            }
            if ((++player->tetherFrame) > 15)
                ActorSequence_Begin(player, 7, 2);
            break;

        case 2:
            Player_AddControlFlags(player, 4);
            player->terrainRecoveryCounter = (player->tetherFrame * 200) + 160;
            if ((++player->tetherFrame) > 7) {
                Player_AddControlFlags(player, 1);
                ActorSequence_Begin(player, 0, 0);
                player->triggerAnimationCallback = PlayerCue_UpdateHidden;
                PlayerCue_CompleteAction(player);
            }
            break;

        case 3:
            if (!player->tetherFrame) {
                player->facing = 4;
                Player_UpdateFacingSide(player);
            }
            Anim_SetSequence(&player->cueCollider.sequence, player->cueCollider.alternateResource,
                             player->movementMode46B, 0, 0);
            PlayerCue_StepSequence((struct ContactSwarmSprite *)&player->secondaryDrawFlags,
                                   &player->cueCollider.sequence);
            Player_AddControlFlags(player, 4);
            player->secondaryDrawFlags |= 2;
            player->secondaryScaleX = 4;
            player->secondaryScaleY = 120;
            player->terrainRecoveryCounter = ((8 - player->tetherFrame) * 200) + 160;
            if ((++player->tetherFrame) > 7)
                ActorSequence_Begin(player, 7, 4);
            break;

        case 4:
            Anim_SetSequence(&player->cueCollider.sequence, player->cueCollider.alternateResource,
                             player->movementMode46B, 0, 0);
            PlayerCue_StepSequence((struct ContactSwarmSprite *)&player->secondaryDrawFlags,
                                   &player->cueCollider.sequence);
            player->secondaryDrawFlags |= 2;
            player->secondaryScaleX = 64 - ((15 - ((u8)player->tetherFrame)) << 2);
            if (player->tetherFrame <= 7) {
                player->secondaryScaleY = (15 - player->tetherFrame) << 3;
                player->terrainRecoveryCounter = (8 - player->tetherFrame) * 20;
            } else {
                player->secondaryScaleY = 64;
                player->terrainRecoveryCounter = 0;
                Player_AddControlFlags(player, 4);
            }
            if ((++player->tetherFrame) > 15)
                ActorSequence_Begin(player, 7, 5);
            break;

        case 5:
            player->secondaryDrawFlags &= ~2;
            Anim_SetSequence(&player->cueCollider.sequence, player->cueCollider.alternateResource,
                             player->movementMode46B, 0, 0);
            PlayerCue_StepSequence((struct ContactSwarmSprite *)&player->secondaryDrawFlags,
                                   &player->cueCollider.sequence);
            if ((++player->tetherFrame) > 3) {
                Player_UpdateFacingSide(player);
                PlayerCue_EnterStandUpdate(player);
                player->triggerAnimationCallback = PlayerCue_RestartAlternateAnimation;
                PlayerCue_CompleteAction(player);
            }
            break;
    }
}

// confidence=low: caller selects this update for player state five.

static inline u32 GetStatBonusFlags(struct Player *player, u32 mask)
{
    if (!player)
        return 0;
    {
        u32 *events = &player->statBonusFlags;
        u32 result = mask;
        result &= *events;
        return result;
    }
}

static inline u32 GetActionFlags(struct Player *player, u32 mask)
{
    if (!player)
        return 0;
    {
        u32 *events = &player->actionFlags;
        u32 result = mask;
        result &= *events;
        return result;
    }
}

static inline s32 HasHighStatBonusFlag(struct Player *actor)
{
    if (!actor)
        return 0;
    return (s32)actor->statBonusFlags < 0;
}

void Player_UpdateStateMotionCue(struct Player *player)
{
    if (!(player->tetherFrame)) {
        Sound_PlaySong(490);
        if (player->scriptStatus[0])
            player->spriteResource = 45;
        else if (*(gGameState->counters628 + player->timedEventSlot) > 0)
            player->spriteResource = 46;
        else if (GetStatBonusFlags(player, 0x10000000))
            player->spriteResource = 0x179;
        else if (HasHighStatBonusFlag(player))
            player->spriteResource = 0x175;
        else if (GetActionFlags(player, 1))
            player->spriteResource = 0x177;
        else if (GetStatBonusFlags(player, 0x40000000))
            player->spriteResource = 0x178;
        else if (GetStatBonusFlags(player, 0x20000000))
            player->spriteResource = 0x176;
        else
            player->spriteResource = 51;
    }
    Player_AddControlFlags(player, 64);
    Player_SetAnimation(player, 154, 64, 1);
    player->previousPosition = player->position;
    player->actionVariantResource = (u32)(gPaletteBankData + ((player->spriteResource) * 32));
    if ((player->tetherFrame) <= 9) {
        struct DisplaySequenceState *s = &player->cueCollider.sequence;
        struct ContactSwarmSprite *draw;
        Anim_SetSequence(s, (void *)(player->cueCollider.alternateResource),
                         (player->movementMode46B) + 9, 0, 0);
        draw = (struct ContactSwarmSprite *)&player->secondaryDrawFlags;
        PlayerCue_StepSequence(draw, s);
    } else if (((player->tetherFrame) >> 2) & 1)
        Player_AddControlFlags(player, 1);
    if ((++(player->tetherFrame)) > 39) {
        player->actionLatch464 = 1;
        player->actionLatch465 = 1;
        PlayerCue_EnterStandUpdate(player);
        player->pendingAnimationTrigger = 1;
    }
}

// confidence=low: inferred from player cue phases and animation consumers.

// confidence=low: timed held-action phases, animation and completion script.

static inline u32 HasStatusFlag(struct Player *player, u32 mask)
{
    return player->statusFlags & mask;
}

void Player_UpdateTimedChargeAction(struct Player *actor)
{
    struct Player *player = actor;
    Player_AddControlFlags(player, 8);
    switch (player->tetherPhase) {
        case 0:
            Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0), 64, 1);
            if ((player->inputKeys[1]) & 1)
                ActorSequence_Begin(player, 7, 1);
            break;

        case 1: {
            u16 *frame = &player->tetherFrame;
            if (!(*frame)) {
                player->facing = 4;
                player->heading = 1;
                player->side = 0;
                Player_SetActionOrigin(player);
                (*frame)++;
            }
            player->resetValue6FC = 15;
            if (!Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 8), 64,
                                     1))
                break;
            Sound_PlaySong(203);
            if (!((player->inputKeys[0]) & 1)) {
                ActorSequence_Begin(player, 7, 4);
                break;
            }
            if (HasStatusFlag(player, 16)) {
                Sound_PlaySong(216);
                player->chargeSoundActive = 1;
                player->chargeCounter = 0;
                ActorSequence_Begin(player, 7, 2);
                break;
            }
            player->chargeCounter = 0;
            {
                ActorSequence_Begin(player, 7, 3);
                break;
            }
        }

        case 2: {
            u32 flags = 160;
            u16 *count;
            player->statusFlags |= flags;
            count = &player->chargeCounter;
            if ((*count) <= 4) {
                if (((++(*count)) == 5) && (!(player->actionMode)))
                    Sound_PlaySong(296);
            }
            Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 9), 64, 1);
            player->resetValue6FC = 30;
            ConvergingEffects_EmitNearParticle(player, &player->actionOrigin);
            if (!((player->inputKeys[0]) & 1)) {
                Sound_StopSong(216);
                player->chargeSoundActive = 0;
                {
                    ActorSequence_Begin(player, 7, 4);
                    break;
                }
            }
            if (!HasStatusFlag(player, 16)) {
                Sound_StopSong(216);
                player->chargeSoundActive = 0;

                ActorSequence_Begin(player, 7, 3);

                break;
            }
            {
                s32 elapsed = gGameState->unk876;
                elapsed += player->cueElapsed;
                player->cueElapsed = elapsed;
                if ((u16)elapsed >= (player->cueRequired)) {
                    u32 *scriptPtr;
                    u32 script;
                    Hud_ClearTimer();
                    ActorSequence_Begin(player, 7, 5);
                    scriptPtr = &player->cueParameter;
                    script = *scriptPtr;
                    if (script) {
                        s32 value;
                        struct ScriptUtilityArguments call;
                        call.count = 1;
                        call.values = &value;
                        value = 1;
                        *scriptPtr = 0;
                        Script_Call(script, &call);
                    }
                }
                break;
            }
        }

        case 3: {
            u32 flags = 128;
            player->statusFlags |= flags;
            Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 10), 64, 1);
            player->resetValue6FC = 30;
            if (!((player->inputKeys[0]) & 1)) {

                ActorSequence_Begin(player, 7, 4);

                break;
            }
            if (HasStatusFlag(player, 16)) {
                Sound_PlaySong(216);
                player->chargeSoundActive = 1;
                player->chargeCounter = 0;
                ActorSequence_Begin(player, 7, 2);
            }
            break;
        }
        case 4:
            player->resetValue6FC = 15;
            if (Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 11), 64,
                                    1)) {
                Player_UpdateFacingSide(player);
                ActorSequence_Begin(player, 7, 0);
            }
            break;

        case 5:
            Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 9), 64, 1);
            player->resetValue6FC = 30;
            ConvergingEffects_EmitNearParticle(player, &player->actionOrigin);
            break;
    }

    if ((player->tetherPhase) != 5) {
        if (!(--(player->cueRemaining))) {
            u32 *scriptPtr;
            u32 script;
            Hud_ClearTimer();
            Sound_StopSong(216);
            Player_UpdateFacingSide(player);
            PlayerCue_EnterStandUpdate(player);
            scriptPtr = &player->cueParameter;
            script = *scriptPtr;
            if (script) {
                s32 value;
                struct ScriptUtilityArguments call;
                call.count = 1;
                call.values = &value;
                value = 0;
                *scriptPtr = 0;
                Script_Call(script, &call);
            }
        }
    }
}

// confidence=low: timed held-action phases, animation and completion script.

static inline void AddStatusFlags(u32 *flags, u32 mask)
{
    *flags |= mask;
}

static inline u32 TestStatusFlags(struct Player *player, u32 mask)
{
    return player->statusFlags & mask;
}

// confidence=low: sibling of timed charge action with reversed hold gating and a pulsing overlay.

void PlayerCue_UpdateTimedChargePulse(struct Player *player)
{
    Player_AddControlFlags(player, 8);
    switch (player->tetherPhase) {
        case 0:
            Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 0), 64, 1);
            if (player->inputKeys[1] & 1)
                ActorSequence_Begin(player, 7, 1);
            break;

        case 1: {
            u16 *frame = &player->tetherFrame;
            if (!(*frame)) {
                player->facing = 4;
                player->heading = 1;
                player->side = 0;
                Player_SetActionOrigin(player);
                (*frame)++;
            }
            player->resetValue6FC = 15;
            if (!Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 8), 64,
                                     1))
                break;
            Sound_PlaySong(0xCB);
            if (!(player->inputKeys[0] & 1)) {
                ActorSequence_Begin(player, 7, 4);
                break;
            }
            if (!TestStatusFlags(player, 16)) {
                Sound_PlaySong(0x2E4);
                player->chargeSoundActive = 1;
                player->chargeCounter = 0;
                ActorSequence_Begin(player, 7, 2);
                break;
            }
            player->chargeCounter = 0;
            {
                ActorSequence_Begin(player, 7, 3);
                break;
            }
        }

        case 2: {
            u16 *count;
            AddStatusFlags(&player->statusFlags, 0xA0);
            count = &player->chargeCounter;
            if ((*count) <= 4) {
                if ((++(*count)) == 5)
                    Sound_PlaySong(0x239);
            }
            Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 9), 64, 1);
            player->resetValue6FC = 30;
            ConvergingEffects_EmitFarParticle(player, &player->actionOrigin);
            player->actionEighteenParameter = 0x16F;
            if ((player->tetherFrame) <= 31)
                player->actionEighteenDuration = player->tetherFrame;
            else
                player->actionEighteenDuration = 64 - (player->tetherFrame);
            if ((++(player->tetherFrame)) > 64)
                player->tetherFrame = 0;
            if (!(player->inputKeys[0] & 1)) {
                Sound_StopSong(0x239);
                player->chargeSoundActive = 0;
                {
                    ActorSequence_Begin(player, 7, 4);
                    break;
                }
            }
            if (TestStatusFlags(player, 16)) {
                Sound_StopSong(0x239);
                player->chargeSoundActive = 0;

                ActorSequence_Begin(player, 7, 3);

                break;
            }
            {
                if ((player->cueElapsed += 4) >= player->cueRequired) {
                    u32 *scriptPtr;
                    u32 script;
                    Hud_ClearTimer();
                    player->tetherPhase = 5;
                    scriptPtr = &player->cueParameter;
                    script = *scriptPtr;
                    if (script) {
                        s32 value;
                        struct ScriptUtilityArguments call;
                        call.count = 1;
                        call.values = &value;
                        value = 1;
                        *scriptPtr = 0;
                        Script_Call(script, &call);
                    }
                }
                break;
            }
        }

        case 3:
            AddStatusFlags(&player->statusFlags, 0x80);
            Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 10), 64, 1);
            player->resetValue6FC = 30;
            if (!(player->inputKeys[0] & 1)) {

                ActorSequence_Begin(player, 7, 4);

                break;
            }
            if (!TestStatusFlags(player, 16)) {
                Sound_PlaySong(0x239);
                player->chargeSoundActive = 1;
                player->chargeCounter = 0;
                ActorSequence_Begin(player, 7, 2);
            }
            break;

        case 4:
            player->resetValue6FC = 15;
            if (Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 11), 64,
                                    1)) {
                Player_UpdateFacingSide(player);
                ActorSequence_Begin(player, 7, 0);
            }
            break;

        case 5:
            player->actionEighteenParameter = 0x16F;
            if ((player->tetherFrame) <= 31)
                player->actionEighteenDuration = player->tetherFrame;
            else
                player->actionEighteenDuration = 64 - (player->tetherFrame);
            if ((++(player->tetherFrame)) > 64)
                player->tetherFrame = 0;
            Player_SetAnimation(player, ActorVariant_GetTableValue(player->actionMode, 9), 64, 1);
            player->resetValue6FC = 30;
            ConvergingEffects_EmitFarParticle(player, &player->actionOrigin);
            break;
    }

    if ((player->tetherPhase) != 5) {
        if (!(--(player->cueRemaining))) {
            u32 *scriptPtr;
            u32 script;
            Hud_ClearTimer();
            Sound_StopSong(0x239);
            Player_UpdateFacingSide(player);
            PlayerCue_EnterStandUpdate(player);
            scriptPtr = &player->cueParameter;
            script = *scriptPtr;
            if (script) {
                s32 value;
                struct ScriptUtilityArguments call;
                call.count = 1;
                call.values = &value;
                value = 0;
                *scriptPtr = 0;
                Script_Call(script, &call);
            }
        }
    }
}

void PlayerCue_UpdateStandDiagonalB(struct Player *player)
{
    if (Player_SetAnimation(player, 0x1BB, 0x40, 1) != 0) {
        player->tetherPhase++;
        if (player->tetherPhase > 1) {
            Player_UpdateFacingSide(player);
            PlayerCue_EnterStandUpdate(player);
            PlayerCue_CompleteAction(player);
        }
    }
}

extern u32 gUnk_0200051C, gUnk_02000524;

static inline void ClearAnimationFlags(struct PlayerAnimation *animation, u32 mask)
{
    u32 flags = animation->flags;
    flags &= ~mask;
    animation->flags = flags;
}

static inline u32 ClearStatusFlags(u32 *flags, u32 clear)
{
    return *flags & ~clear;
}

static inline u32 TargetDash_ReadParticleRandom(void)
{
    return ParticleRandom_ReadDirectSample(gRandomTable);
}

static inline void TargetDash_SetKeyframes(u32 index, void *anim, void *data, u32 mode, u32 flags)
{
    union TargetDashKeyframeSelection selection;
    selection.raw = index;
    Anim_SetKeyframes(anim, data, selection.sequence, mode, flags);
}

static inline void TargetDash_StartAnimation(struct Player *player, struct PlayerAnimation *anim,
                                             void *data, u32 a, u32 b, u32 mode)
{
    if (player->heading == 0)
        TargetDash_SetKeyframes(a, anim, data, mode, 0);
    else
        TargetDash_SetKeyframes(b, anim, data, mode, 0);
    if (player->side)
        anim->flags |= 4;
    else
        anim->flags &= ~4;
}

static inline void TargetDash_EmitTargetSpark(struct Player *player)
{
    struct ContactPosition vel;
    struct ContactPosition pos;
    struct ContactPosition *v;
    struct ContactPosition *q;
    struct ContactPosition *verticalResult;
    s32 vertical;
    u32 a = TargetDash_ReadParticleRandom() & 0xFF;
    s32 raw = TargetDash_ReadParticleRandom() & 0x1F;
    s32 m;
    v = &vel;
    m = raw - 0x10;
    v->x = TruncateQ12(gUnk_086149C4[(a + 0x40) & 0xFF] * m);
    v->y = 0;
    v->z = TruncateQ12(gUnk_086149C4[a] * m);
    vertical =
        ((ParticleRandom_ReadDirectSample((const u16 *)RANDOM_TABLE_ADDR) & 0x3F) << 1) - 0x30;
    verticalResult = &vel;
    verticalResult->y = vertical;
    q = &pos;
    *q = (*(struct ContactPosition *)&player->cueDestination);
    q->y += 0x1E;
    SceneMovingParticle_Create(&pos, &vel, 60, 1);
}

static inline void TargetDash_EmitRelativeTargetSpark(struct Player *player)
{
    struct ContactPosition vel;
    struct ContactPosition pos;
    struct ContactPosition *v;
    struct ContactPosition *q;
    struct ContactPosition *verticalResult;
    s32 vertical;
    u32 randomOrigin = (u32)&gHeapHead + RESOURCE_REGISTRY_ENTRIES_ADDR;
    const u16 *earlyTable = (const u16 *)(randomOrigin - (u32)&gHeapHead +
                                          (RANDOM_TABLE_ADDR - RESOURCE_REGISTRY_ENTRIES_ADDR));
    u32 sampleOrigin;
    u32 a = ParticleRandom_ReadDirectSample(earlyTable) & 0xFF;
    s32 raw = ParticleRandom_ReadDirectSample(earlyTable) & 0x1F;
    s32 m;
    v = &vel;
    m = raw - 0x10;
    {
        s32 rawX = gUnk_086149C4[(a + 0x40) & 0xFF] * m;
        sampleOrigin = randomOrigin;
        v->x = TruncateQ12(rawX);
    }
    v->y = 0;
    v->z = TruncateQ12(gUnk_086149C4[a] * m);
    vertical =
        ((ParticleRandom_ReadRelativeSample(sampleOrigin, (const u16 *)RANDOM_TABLE_ADDR) & 0x3F)
         << 1) -
        0x30;
    verticalResult = &vel;
    verticalResult->y = vertical;
    q = &pos;
    *q = (*(struct ContactPosition *)&player->cueDestination);
    q->y += 0x1E;
    SceneMovingParticle_Create(&pos, &vel, 60, 1);
}

// confidence=low: Player action "dash to target": 5-phase state machine (approach, run, strike, return, stand)

void PlayerCue_UpdateTargetDash(struct Player *player)
{
    struct PlayerAnimation *anim;
    void *data;
    u32 animationFrame;

    if (Player_ConsumeStateEntry(player)) {
        if (player->unkA88[0] == 0)
            player->unkA88[0] = ParticlePool_Create();
        player->cueSpeed = 0x32;
        gUnk_0200051C = 0;
        gUnk_02000524 = 0;
        (*(struct ContactPosition *)&player->movementStart) =
            (*(struct ContactPosition *)&player->position);
        (*(struct ContactPosition *)&player->alternatePosition) =
            (*(struct ContactPosition *)&player->previousPosition);
        ClearAnimationFlags(&player->secondaryAnimation, 1);
        Player_SetPendingCallback(player, Player_OnTargetMotionExit);
        Player_EmitQueuedEffects(player);
        Player_NotifyActionCategories(player);
    }
    player->secondarySpriteResource = player->spriteResource;
    player->secondaryVariantResource = player->actionVariantResource;
    Player_AddControlFlags(player, 9);
    player->statusFlags = ClearStatusFlags(&player->statusFlags, 0x4000) | 0x60000;

    switch (player->tetherPhase) {
        case 0: {
            u32 frame;
            frame = player->tetherFrame;
            anim = &player->secondaryAnimation;
            data = player->animationData;
            if (frame == 0) {
                Player_SetFacingFromAngle(
                    player,
                    Math_Atan2(
                        player->cueDestination.x - (*(s16 *)(u32)&player->alternatePosition[0]),
                        player->cueDestination.z - (*(s16 *)(u32)&player->alternatePosition[2])));
                TargetDash_StartAnimation(player, anim, data, 0x1A1, 0x1A6, 1);
            }
            Anim_Step(anim, data);
            player->tetherFrame++;
            if (player->tetherFrame > 10)
                Player_SetActionPhase(player, 1);
            break;
        }
        case 1: {
            s32 deltaX, deltaZ, angle, facingQuadrant;
            u32 frame;
            deltaX = player->cueDestination.x - (*(s16 *)(u32)&player->alternatePosition[0]);
            deltaZ = player->cueDestination.z - (*(s16 *)(u32)&player->alternatePosition[2]);
            angle = Math_Atan2(deltaX, deltaZ);
            ParticlePool_EmitTriple(player->unkA88[0],
                                    (struct ContactPosition *)&player->alternatePosition,
                                    (angle + 0x80) & 0xFF, 0);
            frame = player->tetherFrame;
            anim = &player->secondaryAnimation;
            data = player->animationData;
            if (frame == 0) {
                Player_SetFacingFromAngle(player, angle);
                Sound_PlaySong(0x4F5);
                TargetDash_StartAnimation(player, anim, data, 0x1A2, 0x1A7, 1);
            }
            Anim_Step(anim, data);
            if (deltaX * deltaX + deltaZ * deltaZ < player->cueSpeed * player->cueSpeed) {
                player->facing = (((((angle + 0x20) & 0xFF) >> 6) << 1) + 3) & 7;
                player->alternatePosition[1] = player->cueDestination.y;
                Player_SetFacingFromAngle(player, angle);
                Player_SetActionPhase(player, 2);
                TargetDash_EmitTargetSpark(player);
            } else {
                (*(s16 *)(u32)&player->alternatePosition[0]) +=
                    TruncateQ12(gUnk_086149C4[(angle + 0x40) & 0xFF] * player->cueSpeed);
                {
                    u32 speed = player->cueSpeed;
                    (*(s16 *)(u32)&player->alternatePosition[2]) +=
                        TruncateQ12(gUnk_086149C4[angle & 0xFF] * speed);
                }
                player->facing = (((((angle + 0x20) & 0xFF) >> 6) << 1) + 3) & 7;
                if (Div(player->movementDuration, 2) < (s32)player->tetherFrame) {
                    (*(s16 *)(u32)&player->alternatePosition[1]) -= 0x50;
                    if ((*(s16 *)(u32)&player->alternatePosition[1]) < player->cueDestination.y)
                        player->alternatePosition[1] = player->cueDestination.y;
                } else {
                    (*(s16 *)(u32)&player->alternatePosition[1]) += 0x50;
                    if ((*(s16 *)(u32)&player->alternatePosition[1]) >
                        player->cueDestination.y + 1000)
                        player->alternatePosition[1] = player->cueDestination.y + 1000;
                }
                facingQuadrant = ((angle + 0x60) & 0xFF) >> 6;
                if (facingQuadrant > 1) {
                    player->heading = 3 - facingQuadrant;
                    player->side = 1;
                } else {
                    player->heading = facingQuadrant;
                    player->side = 0;
                }
                player->tetherFrame++;
                if (player->tetherFrame > 0x59)
                    Player_SetActionPhase(player, 3);
            }
            if (player->inputKeys[1] & 2)
                gUnk_02000524 = 1;
            PlayerCue_InitAttackCollider(
                player, (struct ContactPosition *)&player->alternatePosition, 1, 0x20);
            break;
        }
        case 2: {
            s32 deltaX, deltaZ, angle;
            if (player->tetherFrame == 0) {
                Sound_PlaySong(0x4FB);
                deltaX = player->cueDestination.x - (*(s16 *)(u32)&player->alternatePosition[0]);
                deltaZ = player->cueDestination.z - (*(s16 *)(u32)&player->alternatePosition[2]);
                if (deltaX * deltaX + deltaZ * deltaZ > 0x9C4) {
                    angle = Math_Atan2(deltaX, deltaZ);
                    Player_SetFacingFromAngle(player, angle);
                    player->alternatePosition[0] =
                        player->cueDestination.x +
                        TruncateQ12(gUnk_086149C4[(angle + 0x40) & 0xFF] * 0x32);
                    player->alternatePosition[2] =
                        player->cueDestination.z + TruncateQ12(gUnk_086149C4[angle & 0xFF] * 0x32);
                }
                gUnk_0200051C = 0;
            }
            if (player->tetherFrame == 5) {
                struct PlayerAnimation *animation;
                if (player->heading == 0) {
                    animation = &player->secondaryAnimation;
                    Anim_SetKeyframes(animation, player->animationData, 0x1A3, 2, 0);
                } else {
                    animation = &player->secondaryAnimation;
                    Anim_SetKeyframes(animation, player->animationData, 0x1A8, 2, 0);
                }
                if (player->side != 0)
                    animation->flags |= 4;
                else
                    animation->flags &= ~4;
            }
            if ((player->tetherFrame & 1) == 0)
                TargetDash_EmitRelativeTargetSpark(player);
            player->tetherFrame++;
            if (player->tetherFrame > 0x19) {
                Player_SetActionPhase(player, 3);
                break;
            }
            if (player->inputKeys[1] & 2)
                gUnk_0200051C = 1;
            break;
        }
        case 3: {
            s32 deltaX, deltaZ, angle;
            deltaX = player->position.x - (*(s16 *)(u32)&player->alternatePosition[0]);
            deltaZ = player->position.z - (*(s16 *)(u32)&player->alternatePosition[2]);
            angle = Math_Atan2(deltaX, deltaZ);
            if (deltaX * deltaX + deltaZ * deltaZ <= 0xE0F) {
                (*(struct ContactPosition *)&player->alternatePosition) =
                    (*(struct ContactPosition *)&player->position);
                Player_SetActionPhase(player, 4);
            } else {
                (*(s16 *)(u32)&player->alternatePosition[0]) +=
                    TruncateQ12(gUnk_086149C4[(angle + 0x40) & 0xFF] * 0x32);
                (*(s16 *)(u32)&player->alternatePosition[2]) +=
                    TruncateQ12(gUnk_086149C4[angle & 0xFF] * 0x32);
                player->tetherFrame++;
                if (player->tetherFrame > 0x59)
                    Player_SetActionPhase(player, 4);
            }
            animationFrame = player->tetherFrame;
            anim = &player->secondaryAnimation;
            data = player->animationData;
            if (animationFrame == 0)
                TargetDash_StartAnimation(player, anim, data, 0x1A4, 0x1A9, 2);
            Anim_Step(anim, data);
            player->tetherFrame++;
            break;
        }
        case 4:
            animationFrame = player->tetherFrame;
            anim = &player->secondaryAnimation;
            data = player->animationData;
            if (animationFrame == 0)
                TargetDash_StartAnimation(player, anim, data, 0x1A4, 0x1A9, 1);
            Anim_Step(anim, data);
            if (player->tetherFrame > 10) {
                ActorSequence_Begin(player, 0, 0);
                Player_UpdateFacingSide(player);
                PlayerCue_EnterStandUpdate(player);
                PlayerCue_CompleteAction(player);
            }
            player->tetherFrame++;
            break;
    }
}
