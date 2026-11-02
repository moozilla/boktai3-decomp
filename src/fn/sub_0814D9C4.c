#include "global.h"
#include "player.h"

// MATCHING_CONCESSION: register-pinning
// confidence=low: table-driven weapon cue and combo phases drive the attack collider.
#define Player_UpdateWeaponCueCombo sub_0814D9C4
struct PlayerActionCueInfo {
    u8 unknown0[0x15];
    u8 cueIndex;
    u8 unknown16[0xA];
};
struct WeaponCuePhase {
    u8 kind, forced;
    u16 unknown2;
    u16 delay, speed, unknown8, releaseDuration;
};
struct WeaponCueRecord {
    u16 speed, unknown2;
    struct WeaponCuePhase phases[5];
};
extern const struct WeaponCueRecord gUnk_08E5FA6C[];
void ActionRecord_CopyFromEntry(void *, void *);
void Player_AddControlFlags(void *, u32);
void Player_SelectActionVariant(void *);
s32 Player_SelectActionCue(void *);
s32 Player_SetAnimation(void *, s32, u32, u32);
void Player_SetActionPhase(void *, u8);
void FrameDisplay_UpdateSprite(void *);
void Player_UpdateAttackFacing(void *);
void Player_InitializeActionContact(void *);
s32 Player_UpdateStatThreshold(void *);
s32 Player_IsActionCueReady(void *);
s32 Player_HasWeaponComboEntry(void *);
void sub_0814D264(void *, s32);
void ActorSequence_Begin(void *, u32, u32);
static inline void InvalidatePreviousAnimation(struct Player *p)
{
    s16 *field = &p->statModifierContext.previousAnimation;
    s16 missing = -1;
    *field = missing;
}
static inline void ClearContactTimer(struct Player *p)
{
    u16 *timer = &p->statModifierContext.contactTimer;
    u32 zero = 0;
    *timer = zero;
}
void Player_UpdateWeaponCueCombo(struct Player *p)
{
    struct PlayerActionCueInfo info;
    const struct WeaponCueRecord *record;
    s32 forced = 0;
    ActionRecord_CopyFromEntry(p->statModifierContext.actionEntry, &info);
    record = &gUnk_08E5FA6C[info.cueIndex];
    Player_AddControlFlags(p, 0x100);
    switch (p->tetherPhase) {
        case 0: {
            u16 *timer = &p->tetherFrame;
            if (!(*timer))
                Player_SelectActionVariant(p);
            if (Player_SetAnimation(p, Player_SelectActionCue(p), record->speed, 2)) {
                p->actionResetCounter = 0;
                Player_SetActionPhase(p, 1);
            }
            ++(*timer);
            FrameDisplay_UpdateSprite(p->actionDisplay);
            break;
        }

        case 1:
            Player_UpdateAttackFacing(p);
            Player_SetAnimation(p, Player_SelectActionCue(p), record->speed, 2);
            if (!((*(p->inputKeys)) & 2)) {
                Player_InitializeActionContact(p);
                Player_SetActionPhase(p, 2);
            } else
                ++(p->tetherFrame);
            break;

        case 2: {
            register u32 offset asm("r0") = p->actionResetCounter * sizeof(struct WeaponCuePhase);
            register const struct WeaponCuePhase *phase asm("r6") =
                (const struct WeaponCuePhase *)((u8 *)record->phases + offset);
            Player_SetAnimation(p, Player_SelectActionCue(p), record->speed, 2);
            if ((++(p->tetherFrame)) > (phase->delay)) {
                InvalidatePreviousAnimation(p);
                Player_SetActionPhase(p, 3);
            }
            if ((p->inputKeys)[1] & 2)
                p->statModifierContext.bufferedAttack = 1;
            ClearContactTimer(p);
            break;
        }

        case 3: {
            u32 offset = p->actionResetCounter * sizeof(struct WeaponCuePhase);
            const struct WeaponCuePhase *phase =
                (const struct WeaponCuePhase *)((u8 *)record->phases + offset);
            if (!(p->tetherFrame)) {
                s32 cost = Player_UpdateStatThreshold(p);
                if ((p->currentStatB) <= cost)
                    p->currentStatB = 0;
                else
                    p->currentStatB -= cost;
            }
            ++(p->tetherFrame);
            p->statModifierContext.contactParameter = 10;
            if ((p->inputKeys)[1] & 2)
                p->statModifierContext.bufferedAttack = 1;
            if (phase->forced)
                forced = 1;
            if (Player_SetAnimation(p, Player_SelectActionCue(p), phase->speed, 2) ||
                Player_IsActionCueReady(p)) {
                if ((p->statModifierContext.bufferedAttack) && Player_HasWeaponComboEntry(p)) {
                    p->statModifierContext.bufferedAttack = 0;
                    ++(p->actionResetCounter);
                    Player_SetActionPhase(p, 2);
                } else
                    Player_SetActionPhase(p, 4);
            }
            sub_0814D264(p, forced);
            break;
        }

        case 4: {
            register u32 offset asm("r0") = p->actionResetCounter * sizeof(struct WeaponCuePhase);
            register const struct WeaponCuePhase *phase asm("r6") =
                (const struct WeaponCuePhase *)((u8 *)record->phases + offset);
            ++(p->tetherFrame);
            Player_SetAnimation(p, Player_SelectActionCue(p), phase->speed, 2);
            if ((p->tetherFrame) > (phase->releaseDuration))
                Player_SetActionPhase(p, 5);
            break;
        }

        case 5: {
            u8 *pending = &p->statModifierContext.bufferedAttack;
            u32 zero = 0;
            *pending = zero;
            if (Player_SetAnimation(p, Player_SelectActionCue(p), record->speed, 2) ||
                ((p->tetherFrame) > 30)) {
                ActorSequence_Begin(p, 0, 0);
                p->actionResetCounter = zero;
            }
            break;
        }

        default:
            ActorSequence_Begin(p, 0, 0);
            break;
    }
}
