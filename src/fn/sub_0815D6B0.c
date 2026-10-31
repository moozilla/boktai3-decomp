#include "player_cue.h"

void PlayerCue_BeginAcknowledgement(struct Player *);
void PlayerCue_AcknowledgePendingTrigger(struct Player *);
void PlayerCue_Hide(struct Player *);
void PlayerCue_SetPosition(struct Player *, const struct ContactPosition *);
void PlayerCue_Stand(struct Player *, s32);
void PlayerCue_Face(struct Player *, s32, s32);
void PlayerCue_Idle(struct Player *, s32);
void PlayerCue_StandVariant2(struct Player *, s32, s32);
void PlayerCue_StandVariant3(struct Player *, s32, s32);
void PlayerCue_WalkTo(struct Player *, const struct ContactPosition *, s32, s32);
void PlayerCue_Motion7Front(struct Player *, s32, s32);
void PlayerCue_Motion7(struct Player *, s32, s32);
void PlayerCue_Motion7End(struct Player *, s32);
void PlayerCue_Kneel(struct Player *, s32, s32);
void PlayerCue_KneelDown(struct Player *, s32);
void PlayerCue_KneelGasp(struct Player *, s32);
void PlayerCue_GetUp(struct Player *, s32, s32);
void PlayerCue_MoveTo(struct Player *, const struct ContactPosition *, s32, s32, s32);
void PlayerCue_Motion29(struct Player *, s32, s32, s32);
void PlayerCue_Motion30(struct Player *, s32, s32, s32, s32);
void PlayerCue_Motion30End(struct Player *, s32);
void PlayerCue_Motion29B(struct Player *, s32, s32, s32);
void PlayerCue_Motion7Turn(struct Player *, s32, s32);
void PlayerCue_Motion7Variant3(struct Player *, s32, s32);
void PlayerCue_Motion7Variant7(struct Player *, s32, s32);
void PlayerCue_Aim(struct Player *, s32, s32);
void PlayerCue_Shoot(struct Player *, s32, s32);
void PlayerCue_AimHold(struct Player *, s32, s32, s32);
void PlayerCue_AimEnd(struct Player *, s32);
void PlayerCue_Motion11(struct Player *, s32);
s32 PlayerCue_Motion10(struct Player *, s32);
s32 PlayerCue_FormMotion(struct Player *, s32);
u32 PlayerCue_Motion38To(struct Player *, const struct SceneVec *, u32);
s32 PlayerCue_StateMotion(struct Player *, s32);
void PlayerCue_RunTo(struct Player *, const struct ContactPosition *, s32, s32);
void PlayerCue_Run(struct Player *, s32);
void PlayerCue_Windblown(struct Player *, s32, s32);
void PlayerCue_WindblownVariant3(struct Player *, s32, s32);
void PlayerCue_WindblownVariant4(struct Player *, s32, s32);
void PlayerCue_WindblownVariant2(struct Player *, s32, s32);
void PlayerCue_StandDiagonal(struct Player *, s32, s32);
void PlayerCue_StandDiagonalB(struct Player *, s32, s32);
void PlayerCue_FaceSide(struct Player *, s32);
void PlayerCue_Motion17(struct Player *, s32);
void PlayerCue_Motion17Variant2(struct Player *, s32);
void PlayerCue_ShakeEffect(struct Player *, s32);
void PlayerCue_Refresh(struct Player *);
void PlayerCue_OverlaySet(struct Player *, s32);
void PlayerCue_OverlayOn(struct Player *, s32);
void PlayerCue_OverlayOff(struct Player *);
void sub_0815C73C(struct Player *, s32, s32, s32);

static inline s32 PlayerCue_ReadArgument(const struct SceneEventCommand *command, u32 index)
{
    const s16 *arguments = &command->value;
    return arguments[index];
}

void PlayerCue_Dispatch(struct Player *p)
{
    struct SceneEventReceiver *entry = &PlayerCue_Messages(p)->receiver;
    s32 i;
    struct ContactPosition v1, v7, v33;
    struct SceneVec v51;

    i = 0;
    if (i < entry->count[gSceneEventDispatchState->phase])
        do {
            struct SceneEventCommand *command;
            struct SceneEventDispatchState *controller = gSceneEventDispatchState;
            command = entry->pending[controller->phase][i];
            SceneMessages_SetPendingTrigger(entry, command);
            switch (command->kind) {
                case 0:
                    PlayerCue_Hide(p);
                    PlayerCue_BeginAcknowledgement(p);
                    PlayerCue_AcknowledgePendingTrigger(p);
                    break;
                case 1:
                    v1.x = PlayerCue_ReadArgument(command, 0);
                    v1.y = PlayerCue_ReadArgument(command, 1);
                    v1.z = PlayerCue_ReadArgument(command, 2);
                    PlayerCue_SetPosition(p, &v1);
                    PlayerCue_BeginAcknowledgement(p);
                    PlayerCue_AcknowledgePendingTrigger(p);
                    break;
                case 2:
                    PlayerCue_Stand(p, PlayerCue_ReadArgument(command, 0));
                    PlayerCue_BeginAcknowledgement(p);
                    PlayerCue_AcknowledgePendingTrigger(p);
                    break;
                case 3:
                    PlayerCue_Face(p, PlayerCue_ReadArgument(command, 0), 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 4:
                    PlayerCue_Idle(p, PlayerCue_ReadArgument(command, 0));
                    PlayerCue_BeginAcknowledgement(p);
                    PlayerCue_AcknowledgePendingTrigger(p);
                    break;
                case 5:
                    PlayerCue_StandVariant2(p, PlayerCue_ReadArgument(command, 0), 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 6:
                    PlayerCue_StandVariant3(p, PlayerCue_ReadArgument(command, 0), 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 7:
                    v7.x = PlayerCue_ReadArgument(command, 0);
                    v7.y = PlayerCue_ReadArgument(command, 1);
                    v7.z = PlayerCue_ReadArgument(command, 2);
                    PlayerCue_WalkTo(p, &v7, 0, -1);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 8:
                    PlayerCue_Motion7Front(p, PlayerCue_ReadArgument(command, 0), 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 9:
                    PlayerCue_Motion7(p, PlayerCue_ReadArgument(command, 0), 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 10:
                    PlayerCue_Motion7End(p, 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 11:
                    PlayerCue_Kneel(p, PlayerCue_ReadArgument(command, 0), 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 12:
                    PlayerCue_KneelDown(p, PlayerCue_ReadArgument(command, 0));
                    PlayerCue_BeginAcknowledgement(p);
                    PlayerCue_AcknowledgePendingTrigger(p);
                    break;
                case 13:
                    PlayerCue_KneelGasp(p, PlayerCue_ReadArgument(command, 0));
                    PlayerCue_BeginAcknowledgement(p);
                    PlayerCue_AcknowledgePendingTrigger(p);
                    break;
                case 14:
                    PlayerCue_GetUp(p, PlayerCue_ReadArgument(command, 0), 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 15:
                    PlayerCue_MoveTo(p, 0, PlayerCue_ReadArgument(command, 0), 0, 0);
                    PlayerCue_BeginAcknowledgement(p);
                    PlayerCue_AcknowledgePendingTrigger(p);
                    break;
                case 16:
                    PlayerCue_Motion29(p, PlayerCue_ReadArgument(command, 0),
                                       PlayerCue_ReadArgument(command, 1), 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 17:
                    PlayerCue_Motion30(p, PlayerCue_ReadArgument(command, 0),
                                       PlayerCue_ReadArgument(command, 1),
                                       PlayerCue_ReadArgument(command, 2), 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 18:
                    PlayerCue_Motion30End(p, 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 19:
                    PlayerCue_Motion29B(p, PlayerCue_ReadArgument(command, 0),
                                        PlayerCue_ReadArgument(command, 1), 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 20:
                    PlayerCue_Motion7Turn(p, PlayerCue_ReadArgument(command, 0), 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 22:
                    PlayerCue_Motion7Variant3(p, PlayerCue_ReadArgument(command, 0), 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 21:
                    PlayerCue_Motion7Variant7(p, PlayerCue_ReadArgument(command, 0), 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 23:
                    PlayerCue_Aim(p, PlayerCue_ReadArgument(command, 0), 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 24:
                    PlayerCue_Shoot(p, PlayerCue_ReadArgument(command, 0), 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 25:
                    PlayerCue_AimHold(p, PlayerCue_ReadArgument(command, 0),
                                      PlayerCue_ReadArgument(command, 1), 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 26:
                    PlayerCue_AimEnd(p, 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 28:
                    PlayerCue_Motion11(p, 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 29: {
                    s32 r = PlayerCue_Motion10(p, 0);
                    PlayerCue_BeginAcknowledgement(p);
                    if (r == 0)
                        PlayerCue_AcknowledgePendingTrigger(p);
                    break;
                }
                case 30: {
                    s32 r = PlayerCue_FormMotion(p, 0);
                    PlayerCue_BeginAcknowledgement(p);
                    if (r == 0)
                        PlayerCue_AcknowledgePendingTrigger(p);
                    break;
                }
                case 51:
                    v51.x = PlayerCue_ReadArgument(command, 0);
                    v51.y = PlayerCue_ReadArgument(command, 1);
                    v51.z = PlayerCue_ReadArgument(command, 2);
                    PlayerCue_Motion38To(p, &v51, 0);
                    break;
                case 31: {
                    s32 r = PlayerCue_StateMotion(p, 0);
                    PlayerCue_BeginAcknowledgement(p);
                    if (r == 0)
                        PlayerCue_AcknowledgePendingTrigger(p);
                    break;
                }
                case 33:
                    v33.x = PlayerCue_ReadArgument(command, 0);
                    v33.y = PlayerCue_ReadArgument(command, 1);
                    v33.z = PlayerCue_ReadArgument(command, 2);
                    PlayerCue_RunTo(p, &v33, -1, 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 34:
                    PlayerCue_Run(p, PlayerCue_ReadArgument(command, 0));
                    PlayerCue_BeginAcknowledgement(p);
                    PlayerCue_AcknowledgePendingTrigger(p);
                    break;
                case 35:
                    PlayerCue_Windblown(p, PlayerCue_ReadArgument(command, 0), 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 37:
                    PlayerCue_WindblownVariant3(p, PlayerCue_ReadArgument(command, 0), 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 38:
                    PlayerCue_WindblownVariant4(p, PlayerCue_ReadArgument(command, 0), 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 36:
                    PlayerCue_WindblownVariant2(p, PlayerCue_ReadArgument(command, 0), 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 39:
                    PlayerCue_StandDiagonal(p, PlayerCue_ReadArgument(command, 0), 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 40:
                    PlayerCue_StandDiagonalB(p, PlayerCue_ReadArgument(command, 0), 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 27:
                    PlayerCue_FaceSide(p, PlayerCue_ReadArgument(command, 0));
                    PlayerCue_BeginAcknowledgement(p);
                    PlayerCue_AcknowledgePendingTrigger(p);
                    break;
                case 41:
                    PlayerCue_Motion17(p, 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 42:
                    PlayerCue_Motion17Variant2(p, 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 43:
                    PlayerCue_ShakeEffect(p, 2);
                    PlayerCue_BeginAcknowledgement(p);
                    PlayerCue_AcknowledgePendingTrigger(p);
                    break;
                case 44:
                    PlayerCue_Refresh(p);
                    PlayerCue_BeginAcknowledgement(p);
                    PlayerCue_AcknowledgePendingTrigger(p);
                    break;
                case 48:
                    PlayerCue_OverlaySet(p, PlayerCue_ReadArgument(command, 0) + 0x16E);
                    PlayerCue_BeginAcknowledgement(p);
                    PlayerCue_AcknowledgePendingTrigger(p);
                    break;
                case 47:
                    PlayerCue_OverlayOn(p, PlayerCue_ReadArgument(command, 0) + 0x16E);
                    PlayerCue_BeginAcknowledgement(p);
                    PlayerCue_AcknowledgePendingTrigger(p);
                    break;
                case 49:
                    PlayerCue_OverlayOff(p);
                    PlayerCue_BeginAcknowledgement(p);
                    PlayerCue_AcknowledgePendingTrigger(p);
                    break;
                case 50:
                    PlayerCue_OverlaySet(p, 0x17A);
                    PlayerCue_BeginAcknowledgement(p);
                    PlayerCue_AcknowledgePendingTrigger(p);
                    break;
                case 45:
                    PlayerCue_Messages(p)->periodicParticlesEnabled = 1;
                    PlayerCue_BeginAcknowledgement(p);
                    PlayerCue_AcknowledgePendingTrigger(p);
                    break;
                case 46:
                    PlayerCue_Messages(p)->periodicParticlesEnabled = 0;
                    PlayerCue_BeginAcknowledgement(p);
                    PlayerCue_AcknowledgePendingTrigger(p);
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
                            sub_0815C73C(p, PlayerCue_ReadArgument(command, 0), 0, 0);
                            break;
                        case 4:
                        case 5:
                            sub_0815C73C(p, PlayerCue_ReadArgument(command, 0),
                                         PlayerCue_ReadArgument(command, 1),
                                         PlayerCue_ReadArgument(command, 2));
                            break;
                    }
                    PlayerCue_BeginAcknowledgement(p);
                    PlayerCue_AcknowledgePendingTrigger(p);
                    break;
            }
            i++;
        } while (i < entry->count[gSceneEventDispatchState->phase]);
}
