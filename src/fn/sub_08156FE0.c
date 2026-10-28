#include "global.h"
// MATCHING_CONCESSION: register-pinning
#include "player.h"
#include "engine.h"
#include "ram.h"
// confidence=low: action-mode gates and input select the player's next sequence.
struct PlayerPendingNotification {
    void *receiver;
    u32 flags;
    void *payload;
};
void ActorSequence_Begin(struct Player *, s32, s32);
void Player_BeginDirectionSequence(struct Player *, u32);
void Player_StopMovementEffects(struct Player *);
void Player_SubtractGauge430(struct Player *, s32);
void sub_08249244(void *, s32, s32, void *);
s32 Player_TestTetherActionInput(struct Player *);
void Sound_PlaySong(u32);
s32 Trigger_TryOpenNearby(void *);
s32 PlayerBarrier_IsActive(void *);
void PlayerBarrier_Hide(void *);
s32 Player_SelectRequestedActionMode(struct Player *);
s32 Player_GetEventFlags(struct Player *, u32);
void sub_0813B3CC(struct Player *);
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
/* Keep the two input-field address lifetimes separate, as in the ROM. */
static inline const u16 *InitialInput(struct Player *p)
{
    u32 offset = 0x348;
    const u16 **field;
    field = (const u16 **)((u8 *)p + offset);
    return *field;
}
static inline const u16 *ReloadInput(struct Player *p)
{
    register u32 offset asm("r5") = 0x348;
    asm("" : "+r"(offset));
    {
        register const u16 **field asm("r0") = (const u16 **)((u8 *)p + offset);
        return *field;
    }
}
static inline s32 HistoryHead(struct Player *p)
{
    u32 offset = 0x34C;
    u16 *field;
    s32 v;
    field = (u16 *)((u8 *)p + offset);
    v = (s16)field[0];
    return v;
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
    const u16 *keys;
    if ((gEngineFlags & 0x1000) && (u8)(p->tetherMode - 10) <= 1)
        return p->tetherMode;
    if (p->tetherMode != 0x17 && (s8)p->movementAnimationState >= 0 && p->tetherMode != 0x26) {
        p->interactionResetCounter = 0;
        p->directionFrame = 0;
        p->terrainRecoveryCounter = 0;
        ActorSequence_Begin(p, 0x17, 0);
    }
    if (p->tetherMode != 4 && p->unk4E1[2])
        Player_BeginDirectionSequence(p, p->coordinateAxis);
    mode = p->tetherMode;
    if (!p->statGaugeFull && (u8)(p->actionMode - 1) <= 1) {
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
            Player_SubtractGauge430(p, 3);
        else
            Player_SubtractGauge430(p, 4);
    } else {
        if (p->statGaugeFull == 1)
            p->statusValue = 4000;
        else
            p->statusValue = 0;
    }
    if (p->tetherMode == 0x26)
        goto end;
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
        if ((*(struct PlayerPendingNotification **)&gPlayerPendingActor) &&
            gPlayerNotificationPending) {
            if (!((*(struct PlayerPendingNotification **)&gPlayerPendingActor)->flags & 1)) {
                if ((*(struct PlayerPendingNotification **)&gPlayerPendingActor)->flags & 0x10)
                    sub_08249244(
                        (*(struct PlayerPendingNotification **)&gPlayerPendingActor)->receiver, 1,
                        60, (*(struct PlayerPendingNotification **)&gPlayerPendingActor)->payload);
                else
                    sub_08249244(
                        (*(struct PlayerPendingNotification **)&gPlayerPendingActor)->receiver, 1,
                        60, (*(struct PlayerPendingNotification **)&gPlayerPendingActor)->payload);
            }
            (*(struct PlayerPendingNotification **)&gPlayerPendingActor) = 0;
            gPlayerNotificationPending = 0;
        }
        Player_StopMovementEffects(p);
        goto end;
    }
    if (mode - 0x1D > 1 && mode != 0x1B && mode != 0x1C && mode != 0x18 && mode != 0x17 &&
        mode != 0x16 && mode != 0x15 && mode != 0x1A && p->positionResetTimer) {
        ActorSequence_Begin(p, 0x15, 0);
        goto end;
    }
    switch (mode) {
        case 6:
            if (Player_TestTetherActionInput(p))
                goto end;
            Sound_PlaySong(218);
            goto idle;
        default:
            if (p && IsSceneFive(p)) {
                if ((InitialInput(p)[1] & 1) &&
                    (u8)Trigger_TryOpenNearby(&p->positionalEffectGroup))
                    goto idle;
                if (HistoryHead(p) == -1)
                    goto idle;
                if (ReloadInput(p)[0] & 0x100)
                    goto idle;
                ActorSequence_Begin(p, 1, 0);
                goto end;
            }
            {
                register u32 koff asm("r1") = 0x348;
                register const u16 **kf asm("r0");
                asm("" : "+r"(koff));
                kf = (const u16 **)((u8 *)p + koff);
                keys = *kf;
            }
            if (keys[1] & 0x200) {
                register u32 amoff asm("r2") = 0x418;
                register u8 *amp asm("r0");
                u32 am;
                register u32 sgoff asm("r3");
                register u8 *sgp asm("r0");
                u32 sg;
                asm("" : "+r"(amoff));
                amp = (u8 *)p + amoff;
                am = *amp;
                if (am == 6) {
                    ActorSequence_Begin(p, 2, 0);
                    goto end;
                }
                sgoff = 0x45D;
                asm("" : "+r"(sgoff));
                sgp = (u8 *)p + sgoff;
                sg = *sgp;
                if (sg && (u8)(am - 1) <= 1) {
                    Sound_PlaySong(402);
                    goto end;
                }
                if (PlayerBarrier_IsActive(p->actionRequestEffect) && p->actionMode == 0) {
                    PlayerBarrier_Hide(p->actionRequestEffect);
                    goto end;
                }
                mode = Player_SelectRequestedActionMode(p);
                ActorSequence_Begin(p, mode, 0);
                goto end;
            }
            if (keys[1] & 2) {
                if (p->actionMode == 2) {
                    ActorSequence_Begin(p, 38, 0);
                    goto end;
                }
                if (p->actionMode == 1) {
                    ActorSequence_Begin(p, 12, 0);
                    goto end;
                }
                if (Player_GetEventFlags(p, 0x4000))
                    goto cancel;
                if (Player_GetEventFlags(p, 1)) {
                    if (p->statModifierContext.statBOverrideEnabled == 3)
                        goto primary;
                cancel:
                    ActorSequence_Begin(p, 0, 0);
                    sub_0813B3CC(p);
                    goto end;
                }
                goto normalAttack;
            primary:
                ActorSequence_Begin(p, 3, 0);
                goto end;
            normalAttack:
                ActorSequence_Begin(p, 3, 0);
                goto end;
            }
            if (keys[1] & 1) {
                if (p->actionMode == 2) {
                    if (p->historyDirectionMode == 1)
                        ActorSequence_Begin(p, 38, 0);
                    else
                        ActorSequence_Begin(p, 38, 3);
                    goto end;
                }
                if (p->actionMode == 1) {
                    if (p->historyDirectionMode == 1)
                        ActorSequence_Begin(p, 12, 0);
                    else
                        ActorSequence_Begin(p, 12, 3);
                    goto end;
                }
                if (p->actionMode == 0 && Player_TestNearbyInteraction(p)) {
                    Sound_PlaySong(218);
                    ActorSequence_Begin(p, 6, 0);
                    goto end;
                }
                if ((u8)Trigger_TryOpenNearby(&p->positionalEffectGroup))
                    goto idle;
                if (!p->actionMode && PlayerBarrier_IsActive(p->actionRequestEffect)) {
                    ActorSequence_Begin(p, 20, 0);
                    goto end;
                }
                ActorSequence_Begin(p, 7, 0);
                goto end;
            }
            if ((s16)p->directionHistory[0] == -1 || (keys[0] & 0x100) ||
                p->historyDirectionMode == 1) {
            idle:
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
end:
    return 0;
}
