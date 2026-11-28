#include "global.h"
#include "player/player.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "player/cutscene/player_cue_commands.h"
// confidence=low: action-mode gates and input select the player's next sequence.
void ActorSequence_Begin(struct Player *, s32, s32);
void Player_BeginDirectionSequence(struct Player *, u32);
void Player_StopMovementEffects(struct Player *);
void Player_SubtractActionGauge(struct Player *, s32);
void sub_08249244(void *, s32, s32, void *);
s32 Player_TestTetherActionInput(struct Player *);
void Sound_PlaySong(u32);
s32 Trigger_TryOpenNearby(void *);
s32 PileTrap_IsActive(void *);
void PileTrap_Hide(void *);
s32 Player_SelectRequestedActionMode(struct Player *);
s32 Player_GetEventFlags(struct Player *, u32);
void Player_CallCompletionScript(struct Player *);
s32 Player_TestNearbyInteraction(struct Player *);
static inline u32 HasActionFlag(struct Player *p, u32 mask)
{
    if (!p)
        return 0;
    {
        u32 *flags = &p->actionFlags;
        u32 bits = mask;
        return *flags & bits;
    }
}
static inline const u16 *ReloadInput(struct Player *p)
{
    return p->inputKeys;
}
static inline u32 IsSceneFive(struct Player *p)
{
    u32 active = p->active;
    u32 result = 0;
    if (active && p->sceneMode == 5)
        result = 1;
    return result;
}
s32 Player_SelectActionSequence(struct Player *p)
{
    u32 mode;
    if ((gEngineFlags & 0x1000) && (p->tetherMode == 10 || p->tetherMode == 11))
        return p->tetherMode;
    if (p->tetherMode != 0x17 && (s8)p->movementAnimationState >= 0 && p->tetherMode != 0x26) {
        p->interactionResetCounter = 0;
        p->directionFrame = 0;
        p->terrainRecoveryCounter = 0;
        ActorSequence_Begin(p, 0x17, 0);
    }
    if (p->tetherMode != 4 && p->gateCount)
        Player_BeginDirectionSequence(p, p->coordinateAxis);
    mode = p->tetherMode;
    if (!p->statGaugeFull && (p->actionMode == 1 || p->actionMode == 2)) {
        if (!p->statusValue) {
            if (p->contactSpecial && mode != 0x1E)
                ActorSequence_Begin(p, 0x1E, 0);
            Player_StopMovementEffects(p);
            switch (p->tetherMode) {
                default:
                    if (p->actionMode == 2) {
                        if (p->tetherMode != 11)
                            ActorSequence_Begin(p, 11, 0);
                    } else if (p->actionMode == 1) {
                        if (p->tetherMode != 10)
                            ActorSequence_Begin(p, 10, 0);
                    }
                case 4:
                case 12:
                case 21:
                case 24:
                case 25:
                case 26:
                case 28:
                case 38:
                    break;
            }
            p->statusValue = 0;
            p->unk432[0] = 0;
            p->unk432[1] = 1;
            return p->tetherMode;
        }
        if ((HasActionFlag(p, 0x8000) && p->actionMode == 1) ||
            (HasActionFlag(p, 0x10000) && p->actionMode == 2))
            Player_SubtractActionGauge(p, 3);
        else
            Player_SubtractActionGauge(p, 4);
    } else {
        if (p->statGaugeFull == 1)
            p->statusValue = 4000;
        else
            p->statusValue = 0;
    }
    if (p->tetherMode == 0x26)
        return 0;
    if (p->directionFrame) {
        if (p->unk432[1]) {
            p->actionLatch464 = 1;
            p->actionLatch465 = p->unk432[0];
            p->unk432[1] = 0;
        }
        if (p->contactSpecial) {
            if (p->tetherMode != 0x1E)
                ActorSequence_Begin(p, 0x1E, 0);
        } else
            ActorSequence_Begin(p, 0x1D, 0);
        if (gPlayerPendingActor && gPlayerNotificationPending) {
            struct PlayerPendingNotification *notification = gPlayerPendingActor;
            if (!(notification->flags & 1)) {
                if (notification->flags & 0x10)
                    sub_08249244(notification->receiver, 1, 60, notification->callback);
                else
                    sub_08249244(notification->receiver, 1, 60, notification->callback);
            }
            gPlayerPendingActor = 0;
            gPlayerNotificationPending = 0;
        }
        Player_StopMovementEffects(p);
        return 0;
    }
    if (mode - 0x1D > 1 && mode != 0x1B && mode != 0x1C && mode != 0x18 && mode != 0x17 &&
        mode != 0x16 && mode != 0x15 && mode != 0x1A && p->positionResetTimer) {
        ActorSequence_Begin(p, 0x15, 0);
        return 0;
    }
    switch (mode) {
        case 6:
            if (Player_TestTetherActionInput(p))
                break;
            Sound_PlaySong(218);
            ActorSequence_Begin(p, 0, 0);
            break;
        default:
            if (p && IsSceneFive(p)) {
                if (((p->inputKeys[1] & 1) &&
                     (u8)Trigger_TryOpenNearby(&p->positionalEffectGroup)) ||
                    (s16)p->directionHistory[0] == -1 || (ReloadInput(p)[0] & 0x100))
                    ActorSequence_Begin(p, 0, 0);
                else
                    ActorSequence_Begin(p, 1, 0);
                break;
            }
            if (p->inputKeys[1] & 0x200) {
                if (p->actionMode == 6) {
                    ActorSequence_Begin(p, 2, 0);
                    break;
                }
                if (p->statGaugeFull && (p->actionMode == 1 || p->actionMode == 2)) {
                    Sound_PlaySong(402);
                    break;
                }
                if (PileTrap_IsActive(p->actionRequestEffect) && p->actionMode == 0) {
                    PileTrap_Hide(p->actionRequestEffect);
                    break;
                }
                mode = Player_SelectRequestedActionMode(p);
                ActorSequence_Begin(p, mode, 0);
                break;
            }
            if (p->inputKeys[1] & 2) {
                if (p->actionMode == 2) {
                    ActorSequence_Begin(p, 38, 0);
                    break;
                }
                if (p->actionMode == 1) {
                    ActorSequence_Begin(p, 12, 0);
                    break;
                }
                if (Player_GetEventFlags(p, 0x4000))
                    goto cancel;
                if (Player_GetEventFlags(p, 1)) {
                    if (p->statModifierContext.statBOverrideEnabled != 3) {
                    cancel:
                        ActorSequence_Begin(p, 0, 0);
                        Player_CallCompletionScript(p);
                        break;
                    }
                    ActorSequence_Begin(p, 3, 0);
                    break;
                }
                ActorSequence_Begin(p, 3, 0);
                break;
            }
            if (p->inputKeys[1] & 1) {
                if (p->actionMode == 2) {
                    if (p->historyDirectionMode == 1)
                        ActorSequence_Begin(p, 38, 0);
                    else
                        ActorSequence_Begin(p, 38, 3);
                    break;
                }
                if (p->actionMode == 1) {
                    if (p->historyDirectionMode == 1)
                        ActorSequence_Begin(p, 12, 0);
                    else
                        ActorSequence_Begin(p, 12, 3);
                    break;
                }
                if (p->actionMode == 0 && Player_TestNearbyInteraction(p)) {
                    Sound_PlaySong(218);
                    ActorSequence_Begin(p, 6, 0);
                    break;
                }
                if ((u8)Trigger_TryOpenNearby(&p->positionalEffectGroup)) {
                    ActorSequence_Begin(p, 0, 0);
                    break;
                }
                if (!p->actionMode && PileTrap_IsActive(p->actionRequestEffect)) {
                    ActorSequence_Begin(p, 20, 0);
                    break;
                }
                ActorSequence_Begin(p, 7, 0);
                break;
            }
            if ((s16)p->directionHistory[0] == -1 || (p->inputKeys[0] & 0x100) ||
                p->historyDirectionMode == 1) {
                ActorSequence_Begin(p, 0, 0);
            } else
                ActorSequence_Begin(p, 1, 0);
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
        case 26:
        case 27:
        case 28:
        case 29:
        case 30:
        case 37:
        case 38:
            break;
    }
    return 0;
}
