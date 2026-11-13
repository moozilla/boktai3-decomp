#include "player_cue_dispatch.h"

/* The embedded queue is followed by the receiver bookkeeping in Player. */
void Player_RegisterSceneMessageReceiver(struct Player *player)
{
    SceneMessages_RegisterReceiver((struct SceneMessageReceiver *)&player->cueReceiver,
                                   (*(u32 *)&player->positionalEffectGroup), 2);
}

void Player_UnregisterSceneMessageReceiver(struct Player *player)
{
    SceneMessages_UnregisterReceiver((struct SceneMessageReceiver *)&player->cueReceiver);
}

static inline s32 PlayerCue_ReadArgument(const struct SceneEventCommand *command, u32 index)
{
    const s16 *arguments = &command->value;
    return arguments[index];
}

void PlayerCue_Dispatch(struct Player *player)
{
    struct SceneEventReceiver *receiver = &player->cueReceiver;
    s32 index;
    struct ContactPosition position, walkDestination, runDestination;
    struct SceneVec dashDestination;

    index = 0;
    if (index < receiver->count[gSceneEventDispatchState->phase])
        do {
            struct SceneEventCommand *command;
            struct SceneEventDispatchState *controller = gSceneEventDispatchState;
            command = receiver->pending[controller->phase][index];
            SceneMessages_SetPendingTrigger(receiver, command);
            switch (command->kind) {
                case 0:
                    PlayerCue_Hide(player);
                    PlayerCue_BeginAcknowledgement(player);
                    PlayerCue_AcknowledgePendingTrigger(player);
                    break;
                case 1:
                    position.x = PlayerCue_ReadArgument(command, 0);
                    position.y = PlayerCue_ReadArgument(command, 1);
                    position.z = PlayerCue_ReadArgument(command, 2);
                    PlayerCue_SetPosition(player, &position);
                    PlayerCue_BeginAcknowledgement(player);
                    PlayerCue_AcknowledgePendingTrigger(player);
                    break;
                case 2:
                    PlayerCue_Stand(player, PlayerCue_ReadArgument(command, 0));
                    PlayerCue_BeginAcknowledgement(player);
                    PlayerCue_AcknowledgePendingTrigger(player);
                    break;
                case 3:
                    PlayerCue_Face(player, PlayerCue_ReadArgument(command, 0), 0);
                    PlayerCue_BeginAcknowledgement(player);
                    break;
                case 4:
                    PlayerCue_Idle(player, PlayerCue_ReadArgument(command, 0));
                    PlayerCue_BeginAcknowledgement(player);
                    PlayerCue_AcknowledgePendingTrigger(player);
                    break;
                case 5:
                    PlayerCue_StandVariant2(player, PlayerCue_ReadArgument(command, 0), 0);
                    PlayerCue_BeginAcknowledgement(player);
                    break;
                case 6:
                    PlayerCue_StandVariant3(player, PlayerCue_ReadArgument(command, 0), 0);
                    PlayerCue_BeginAcknowledgement(player);
                    break;
                case 7:
                    walkDestination.x = PlayerCue_ReadArgument(command, 0);
                    walkDestination.y = PlayerCue_ReadArgument(command, 1);
                    walkDestination.z = PlayerCue_ReadArgument(command, 2);
                    PlayerCue_WalkTo(player, &walkDestination, 0, -1);
                    PlayerCue_BeginAcknowledgement(player);
                    break;
                case 8:
                    PlayerCue_Motion7Front(player, PlayerCue_ReadArgument(command, 0), 0);
                    PlayerCue_BeginAcknowledgement(player);
                    break;
                case 9:
                    PlayerCue_Motion7(player, PlayerCue_ReadArgument(command, 0), 0);
                    PlayerCue_BeginAcknowledgement(player);
                    break;
                case 10:
                    PlayerCue_Motion7End(player, 0);
                    PlayerCue_BeginAcknowledgement(player);
                    break;
                case 11:
                    PlayerCue_Kneel(player, PlayerCue_ReadArgument(command, 0), 0);
                    PlayerCue_BeginAcknowledgement(player);
                    break;
                case 12:
                    PlayerCue_KneelDown(player, PlayerCue_ReadArgument(command, 0));
                    PlayerCue_BeginAcknowledgement(player);
                    PlayerCue_AcknowledgePendingTrigger(player);
                    break;
                case 13:
                    PlayerCue_KneelGasp(player, PlayerCue_ReadArgument(command, 0));
                    PlayerCue_BeginAcknowledgement(player);
                    PlayerCue_AcknowledgePendingTrigger(player);
                    break;
                case 14:
                    PlayerCue_GetUp(player, PlayerCue_ReadArgument(command, 0), 0);
                    PlayerCue_BeginAcknowledgement(player);
                    break;
                case 15:
                    PlayerCue_MoveTo(player, 0, PlayerCue_ReadArgument(command, 0), 0, 0);
                    PlayerCue_BeginAcknowledgement(player);
                    PlayerCue_AcknowledgePendingTrigger(player);
                    break;
                case 16:
                    PlayerCue_Motion29(player, PlayerCue_ReadArgument(command, 0),
                                       PlayerCue_ReadArgument(command, 1), 0);
                    PlayerCue_BeginAcknowledgement(player);
                    break;
                case 17:
                    PlayerCue_Motion30(player, PlayerCue_ReadArgument(command, 0),
                                       PlayerCue_ReadArgument(command, 1),
                                       PlayerCue_ReadArgument(command, 2), 0);
                    PlayerCue_BeginAcknowledgement(player);
                    break;
                case 18:
                    PlayerCue_Motion30End(player, 0);
                    PlayerCue_BeginAcknowledgement(player);
                    break;
                case 19:
                    PlayerCue_Motion29B(player, PlayerCue_ReadArgument(command, 0),
                                        PlayerCue_ReadArgument(command, 1), 0);
                    PlayerCue_BeginAcknowledgement(player);
                    break;
                case 20:
                    PlayerCue_Motion7Turn(player, PlayerCue_ReadArgument(command, 0), 0);
                    PlayerCue_BeginAcknowledgement(player);
                    break;
                case 22:
                    PlayerCue_Motion7Variant3(player, PlayerCue_ReadArgument(command, 0), 0);
                    PlayerCue_BeginAcknowledgement(player);
                    break;
                case 21:
                    PlayerCue_Motion7Variant7(player, PlayerCue_ReadArgument(command, 0), 0);
                    PlayerCue_BeginAcknowledgement(player);
                    break;
                case 23:
                    PlayerCue_Aim(player, PlayerCue_ReadArgument(command, 0), 0);
                    PlayerCue_BeginAcknowledgement(player);
                    break;
                case 24:
                    PlayerCue_Shoot(player, PlayerCue_ReadArgument(command, 0), 0);
                    PlayerCue_BeginAcknowledgement(player);
                    break;
                case 25:
                    PlayerCue_AimHold(player, PlayerCue_ReadArgument(command, 0),
                                      PlayerCue_ReadArgument(command, 1), 0);
                    PlayerCue_BeginAcknowledgement(player);
                    break;
                case 26:
                    PlayerCue_AimEnd(player, 0);
                    PlayerCue_BeginAcknowledgement(player);
                    break;
                case 28:
                    PlayerCue_Motion11(player, 0);
                    PlayerCue_BeginAcknowledgement(player);
                    break;
                case 29: {
                    s32 started = PlayerCue_Motion10(player, 0);
                    PlayerCue_BeginAcknowledgement(player);
                    if (started == 0)
                        PlayerCue_AcknowledgePendingTrigger(player);
                    break;
                }
                case 30: {
                    s32 started = PlayerCue_FormMotion(player, 0);
                    PlayerCue_BeginAcknowledgement(player);
                    if (started == 0)
                        PlayerCue_AcknowledgePendingTrigger(player);
                    break;
                }
                case 51:
                    dashDestination.x = PlayerCue_ReadArgument(command, 0);
                    dashDestination.y = PlayerCue_ReadArgument(command, 1);
                    dashDestination.z = PlayerCue_ReadArgument(command, 2);
                    PlayerCue_Motion38To(player, &dashDestination, 0);
                    break;
                case 31: {
                    s32 started = PlayerCue_StateMotion(player, 0);
                    PlayerCue_BeginAcknowledgement(player);
                    if (started == 0)
                        PlayerCue_AcknowledgePendingTrigger(player);
                    break;
                }
                case 33:
                    runDestination.x = PlayerCue_ReadArgument(command, 0);
                    runDestination.y = PlayerCue_ReadArgument(command, 1);
                    runDestination.z = PlayerCue_ReadArgument(command, 2);
                    PlayerCue_RunTo(player, &runDestination, -1, 0);
                    PlayerCue_BeginAcknowledgement(player);
                    break;
                case 34:
                    PlayerCue_Run(player, PlayerCue_ReadArgument(command, 0));
                    PlayerCue_BeginAcknowledgement(player);
                    PlayerCue_AcknowledgePendingTrigger(player);
                    break;
                case 35:
                    PlayerCue_Windblown(player, PlayerCue_ReadArgument(command, 0), 0);
                    PlayerCue_BeginAcknowledgement(player);
                    break;
                case 37:
                    PlayerCue_WindblownVariant3(player, PlayerCue_ReadArgument(command, 0), 0);
                    PlayerCue_BeginAcknowledgement(player);
                    break;
                case 38:
                    PlayerCue_WindblownVariant4(player, PlayerCue_ReadArgument(command, 0), 0);
                    PlayerCue_BeginAcknowledgement(player);
                    break;
                case 36:
                    PlayerCue_WindblownVariant2(player, PlayerCue_ReadArgument(command, 0), 0);
                    PlayerCue_BeginAcknowledgement(player);
                    break;
                case 39:
                    PlayerCue_StandDiagonal(player, PlayerCue_ReadArgument(command, 0), 0);
                    PlayerCue_BeginAcknowledgement(player);
                    break;
                case 40:
                    PlayerCue_StandDiagonalB(player, PlayerCue_ReadArgument(command, 0), 0);
                    PlayerCue_BeginAcknowledgement(player);
                    break;
                case 27:
                    PlayerCue_FaceSide(player, PlayerCue_ReadArgument(command, 0));
                    PlayerCue_BeginAcknowledgement(player);
                    PlayerCue_AcknowledgePendingTrigger(player);
                    break;
                case 41:
                    PlayerCue_Motion17(player, 0);
                    PlayerCue_BeginAcknowledgement(player);
                    break;
                case 42:
                    PlayerCue_Motion17Variant2(player, 0);
                    PlayerCue_BeginAcknowledgement(player);
                    break;
                case 43:
                    PlayerCue_ShakeEffect(player, 2);
                    PlayerCue_BeginAcknowledgement(player);
                    PlayerCue_AcknowledgePendingTrigger(player);
                    break;
                case 44:
                    PlayerCue_Refresh(player);
                    PlayerCue_BeginAcknowledgement(player);
                    PlayerCue_AcknowledgePendingTrigger(player);
                    break;
                case 48:
                    PlayerCue_OverlaySet(player, PlayerCue_ReadArgument(command, 0) + 0x16E);
                    PlayerCue_BeginAcknowledgement(player);
                    PlayerCue_AcknowledgePendingTrigger(player);
                    break;
                case 47:
                    PlayerCue_OverlayOn(player, PlayerCue_ReadArgument(command, 0) + 0x16E);
                    PlayerCue_BeginAcknowledgement(player);
                    PlayerCue_AcknowledgePendingTrigger(player);
                    break;
                case 49:
                    PlayerCue_OverlayOff(player);
                    PlayerCue_BeginAcknowledgement(player);
                    PlayerCue_AcknowledgePendingTrigger(player);
                    break;
                case 50:
                    PlayerCue_OverlaySet(player, 0x17A);
                    PlayerCue_BeginAcknowledgement(player);
                    PlayerCue_AcknowledgePendingTrigger(player);
                    break;
                case 45:
                    player->cueParticlesEnabled = 1;
                    PlayerCue_BeginAcknowledgement(player);
                    PlayerCue_AcknowledgePendingTrigger(player);
                    break;
                case 46:
                    player->cueParticlesEnabled = 0;
                    PlayerCue_BeginAcknowledgement(player);
                    PlayerCue_AcknowledgePendingTrigger(player);
                    break;
                case 32:
                    switch (PlayerCue_ReadArgument(command, 0)) {
                        case 0:
                        case 1:
                        case 2:
                        case 3:
                        case 6:
                        case 7:
                        case 8:
                            Player_ConfigureAttachmentEffect(
                                player, PlayerCue_ReadArgument(command, 0), 0, 0);
                            break;
                        case 4:
                        case 5:
                            Player_ConfigureAttachmentEffect(player,
                                                             PlayerCue_ReadArgument(command, 0),
                                                             PlayerCue_ReadArgument(command, 1),
                                                             PlayerCue_ReadArgument(command, 2));
                            break;
                    }
                    PlayerCue_BeginAcknowledgement(player);
                    PlayerCue_AcknowledgePendingTrigger(player);
                    break;
            }
            index++;
        } while (index < receiver->count[gSceneEventDispatchState->phase]);
}

void Player_SendSceneMessage(struct Player *player, u8 command, s32 count, u16 *arguments)
{
    struct PlayerCuePacket *packet = (struct PlayerCuePacket *)player->unkB0C;
    s32 index;
    packet->id = (*(u32 *)&player->positionalEffectGroup);
    packet->receiverKind = 2;
    packet->trigger = 0;
    packet->unknown4 = 0;
    packet->kind = command;
    packet->argumentCount = count;
    if (arguments)
        for (index = 0; index < count; index++)
            packet->arguments[index] = arguments[index];
    SceneMessages_Send((struct SceneEventCommand *)packet);
}

void PlayerCue_InitCollider(struct Player *player)
{
    struct PlayerCueCollider *collider = &player->cueCollider;
    struct PlayerColliderVector size;
    struct PlayerColliderVector offset;
    size.x = 120;
    size.y = 120;
    size.z = 120;
    offset.x = 0;
    offset.y = 120;
    offset.z = 0;
    Collider_Init(collider, 0, 0x2001, 0, (0x10000u << player->movementRequestBlocked) >> 16, &size,
                  &offset);
    Collider_SetAttackProperties(collider, Div(player->particleEmissionCount, 10), 40, 0, 0x40000,
                                 10);
    Collider_SetCallback(collider, 0, player);
    Collider_SetPosition(collider, &player->position, 0);
    player->cueColliderPhase = 6;
    player->cueCollider.direction = ((player->facing + 1) & 7) << 5;
}

void Player_OnStateOneEvent(u32 unusedContact, u32 unusedValue, struct Player *player)
{
    if (player->actionMode == 1)
        gGameState->unk600 += 4;
}

static inline s32 TruncateQ6(s32 x)
{
    if (x >= 0)
        x >>= 6;
    else
        x = -(-x >> 6);
    return x;
}

s32 Player_ScaleLevelValue(u32 flags, s32 scale)
{
    s32 value = gGameState->level + 50;
    s32 adjustment;
    if (flags & 2)
        adjustment = (10 - gGameState->unk876) * (value << 1);
    else {
        s32 amount = gGameState->unk876;
        adjustment = (value << 1) * amount;
    }
    value += Div(adjustment, 100);
    value = TruncateQ6(value * scale);
    return value;
}

void PlayerCue_InitAttackCollider(struct Player *player, void *position, u32 flags, s32 scale)
{
    s32 sum;
    struct PlayerCueCollider *collider = &player->cueCollider;
    struct PlayerColliderVector size;
    struct PlayerColliderVector offset;
    size.x = 120;
    size.y = 120;
    size.z = 120;
    offset.x = 0;
    offset.y = 120;
    offset.z = 0;
    Collider_Init(collider, 0, 0x2001, 0, (0x10000u << player->movementRequestBlocked) >> 16, &size,
                  &offset);
    if (gEngineFlags & 0x1000) {
        s32 bonus = gGameState->unk874;
        s32 level = gGameState->level;
        bonus += 50;
        sum = level + bonus;
        Collider_SetAttackProperties(collider, sum, 40, 16, 0x40000, 10);
    } else
        Collider_SetAttackProperties(collider, Player_ScaleLevelValue(flags, scale), 40, 1,
                                     flags | 0x40000, 10);
    Collider_SetCallback(collider, Player_OnStateOneEvent, player);
    Collider_SetPosition(collider, position, 0);
    player->cueColliderPhase = 6;
    sum = (player->facing + 1) & 7;
    player->cueCollider.direction = sum << 5;
}

void Player_UpdateColliderRefreshCountdown(struct Player *player)
{
    u8 *remaining = &player->cueColliderPhase;
    if (*remaining != 0) {
        Collider_Refresh(&player->cueCollider);
        *remaining = *remaining - 1;
    }
}

static inline u32 ReadMaskedFlags(u32 mask, u32 *flags)
{
    u32 selected = mask;
    selected &= *flags;
    return selected;
}

static inline u32 PlayerCue_TestFlags(struct Player *player, u32 mask)
{
    if (player == 0)
        return 0;
    return ReadMaskedFlags(mask, &player->actionFlags);
}

void PlayerCue_ApplyColliderScale(struct Player *player, void *collider)
{
    s32 scale = 64;
    if (PlayerCue_TestFlags(player, 0x80000))
        scale -= Div(player->derivedStats[1], 5);
    if (PlayerCue_TestFlags(player, 0x40000))
        scale -= 6;
    if (PlayerCue_TestFlags(player, 0x1000000))
        scale += Div(13 * gGameState->unk876, 10);
    if (PlayerCue_TestFlags(player, 0x400000))
        scale -= Div(13 * gGameState->unk876, 10);
    switch (SunClock_GetPhase()) {
        case 1:
        case 2:
        case 3:
            if (PlayerCue_TestFlags(player, 0x100000))
                scale -= 13;
            if (PlayerCue_TestFlags(player, 0x2000000))
                scale += 13;
            break;
        case 0:
        case 4:
        case 5:
            if (PlayerCue_TestFlags(player, 0x200000))
                scale -= 13;
            if (PlayerCue_TestFlags(player, 0x4000000))
                scale += 13;
            break;
    }
    if (scale < 0)
        scale = 1;
    Collider_SetContactMode(collider, scale);
}
