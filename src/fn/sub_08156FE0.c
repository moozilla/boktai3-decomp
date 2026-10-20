#include "global.h"
#include "player.h"

#ifdef NONMATCHING
// fuzzy: 93.4%
// Input-field address reuse, flag-test register allocation and later offsets still differ.
// confidence=low: selects action sequences from gauge, collision and input state.
#define Player_SelectActionSequence sub_08156FE0
extern u32 gUnk_030053F4;
struct PlayerPendingNotification {
    void *receiver;
    u32 flags;
    void *payload;
};
extern struct PlayerPendingNotification *gUnk_0200054C;
extern u16 gUnk_02000594;
void sub_08249244(void *, s32, s32, void *);
void ActorSequence_Begin(struct Player *, s32, s32);
void sub_08158DF0(struct Player *, s32);
void Player_StopMovementEffects(struct Player *);
void Player_SubtractGauge430(struct Player *, s32);
s32 sub_08156F54(struct Player *);
void Sound_PlaySong(s32);
s32 Trigger_TryOpenNearby(void *);
s32 PlayerBarrier_IsActive(void *);
void PlayerBarrier_Hide(void *);
s32 sub_0813F8F8(struct Player *);
s32 Player_GetEventFlags(struct Player *, s32);
void sub_0813B3CC(struct Player *);
s32 Player_TestNearbyInteraction(struct Player *);
static inline u32 ActionFlags(struct Player *p, u32 mask)
{
    if (!p)
        return 0;
    {
        u32 *ptr = &p->actionFlags;
        u32 flags = mask;
        u32 bits = *ptr;
        bits &= flags;
        if (bits)
            return 1;
        return 0;
    }
}
static inline u32 IsScene5(struct Player *p)
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
    if ((gUnk_030053F4 & 0x1000) && (u8)(p->tetherMode - 10) <= 1)
        return p->tetherMode;
    if (p->tetherMode != 0x17 && (s8)p->movementAnimationState >= 0 && p->tetherMode != 0x26) {
        p->interactionResetCounter = 0;
        p->directionFrame = 0;
        p->terrainRecoveryCounter = 0;
        ActorSequence_Begin(p, 0x17, 0);
    }
    if (p->tetherMode != 4 && *((u8 *)p + 0x4E3))
        sub_08158DF0(p, p->coordinateAxis);
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
                    break;
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
        if ((ActionFlags(p, 0x8000) && p->actionMode == 1) ||
            (ActionFlags(p, 0x10000) && p->actionMode == 2))
            Player_SubtractGauge430(p, 3);
        else
            Player_SubtractGauge430(p, 4);
    } else {
        if (p->statGaugeFull == 1)
            p->statusValue = 4000;
        else
            p->statusValue = 0;
    }
    if (p->tetherMode == 38)
        goto done;
    if (p->directionFrame) {
        if (p->unk432[1]) {
            p->actionLatch464 = 1;
            p->actionLatch465 = p->unk432[0];
            p->unk432[1] = 0;
        }
        if (p->contactSpecial) {
            if (p->tetherMode != 30)
                ActorSequence_Begin(p, 30, 0);
        } else
            ActorSequence_Begin(p, 29, 0);
        if (gUnk_0200054C && gUnk_02000594) {
            if (!(gUnk_0200054C->flags & 1)) {
                if (gUnk_0200054C->flags & 0x10)
                    sub_08249244(gUnk_0200054C->receiver, 1, 60, gUnk_0200054C->payload);
                else
                    sub_08249244(gUnk_0200054C->receiver, 1, 60, gUnk_0200054C->payload);
            }
            gUnk_0200054C = 0;
            gUnk_02000594 = 0;
        }
        Player_StopMovementEffects(p);
        goto done;
    }
    if (mode - 29 > 1 && mode != 27 && mode != 28 && mode != 24 && mode != 23 && mode != 22 &&
        mode != 21 && mode != 26 && p->positionResetTimer) {
        ActorSequence_Begin(p, 21, 0);
        goto done;
    }
    switch (mode) {
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
        case 6:
            if (sub_08156F54(p))
                break;
            Sound_PlaySong(218);
            goto zero;
        default:
            if (p && IsScene5(p)) {
                if ((p->inputKeys[1] & 1) && (u8)Trigger_TryOpenNearby(&p->positionalEffectGroup))
                    goto zero;
                if ((s16)p->directionHistory[0] == -1)
                    goto zero;
                if (p->inputKeys[0] & 0x100)
                    goto zero;
                ActorSequence_Begin(p, 1, 0);
                break;
            }
            keys = p->inputKeys;
            if (keys[1] & 0x200) {
                if (p->actionMode == 6) {
                    ActorSequence_Begin(p, 2, 0);
                    break;
                }
                if (p->statGaugeFull && (u8)(p->actionMode - 1) <= 1) {
                    Sound_PlaySong(402);
                    break;
                }
                if (PlayerBarrier_IsActive(p->actionRequestEffect) && p->actionMode == 0) {
                    PlayerBarrier_Hide(p->actionRequestEffect);
                    break;
                }
                mode = sub_0813F8F8(p);
                ActorSequence_Begin(p, mode, 0);
                break;
            }
            if (keys[1] & 2) {
                if (p->actionMode == 2) {
                    ActorSequence_Begin(p, 38, 0);
                    break;
                }
                if (p->actionMode == 1) {
                    ActorSequence_Begin(p, 12, 0);
                    break;
                }
                if (Player_GetEventFlags(p, 0x4000))
                    goto special;
                if (Player_GetEventFlags(p, 1)) {
                    if (p->statModifierContext.statBOverrideEnabled != 3) {
                    special:
                        ActorSequence_Begin(p, 0, 0);
                        sub_0813B3CC(p);
                        break;
                    }
                    ActorSequence_Begin(p, 3, 0);
                    break;
                }
                ActorSequence_Begin(p, 3, 0);
                break;
            }
            if (keys[1] & 1) {
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
                if ((u8)Trigger_TryOpenNearby(&p->positionalEffectGroup))
                    goto zero;
                if (p->actionMode == 0 && PlayerBarrier_IsActive(p->actionRequestEffect)) {
                    ActorSequence_Begin(p, 20, 0);
                    break;
                }
                ActorSequence_Begin(p, 7, 0);
                break;
            }
            if ((s16)p->directionHistory[0] == -1 || (keys[0] & 0x100) ||
                p->historyDirectionMode == 1) {
            zero:
                ActorSequence_Begin(p, 0, 0);
            } else
                ActorSequence_Begin(p, 1, 0);
            break;
    }
done:
    return 0;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_08156FE0);
#endif
