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
extern const u8 gUnk_08E5FA6C[];
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
struct WeaponCuePlayer {
    u8 unk0[0x348];
    const u16 *inputKeys;
    u8 unk34C[0x428 - 0x34C];
    u16 currentStatB;
    u8 unk42A[0x458 - 0x42A];
    u8 phase, unk459;
    u16 timer;
    u8 unk45C[0xAAE - 0x45C];
    u16 comboIndex;
    u8 unkAB0[0xBF0 - 0xAB0];
    void *actionEntry;
    u8 unkBF4[2], comboPending, unkBF7;
    u16 unkBF8[3], contactParameter;
    u8 unkC00[0xA];
    u16 contactTimer;
    u8 unkC0C[0xE];
    u16 historySentinel;
    u8 unkC1C[0x68];
    void *actionDisplay;
};
static inline void SetHistorySentinel(struct WeaponCuePlayer *p)
{
    u16 *field = &p->historySentinel;
    u32 missing = 0xFFFF;
    *field = missing;
}
static inline void ClearContactTimer(struct WeaponCuePlayer *p)
{
    u16 *timer = &p->contactTimer;
    u32 zero = 0;
    *timer = zero;
}
void Player_UpdateWeaponCueCombo(struct WeaponCuePlayer *p)
{
    struct PlayerActionCueInfo info;
    const struct WeaponCueRecord *record;
    s32 forced = 0;
    ActionRecord_CopyFromEntry(p->actionEntry, &info);
    record = (const struct WeaponCueRecord *)gUnk_08E5FA6C + info.cueIndex;
    Player_AddControlFlags(p, 0x100);
    switch (p->phase) {
        case 0: {
            register u16 *timer = &(p->timer);
            if (!(*timer))
                Player_SelectActionVariant(p);
            if (Player_SetAnimation(p, Player_SelectActionCue(p), record->speed, 2)) {
                p->comboIndex = 0;
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
                ++(p->timer);
            break;

        case 2: {
            register u32 offset asm("r0") = p->comboIndex * sizeof(struct WeaponCuePhase);
            register const struct WeaponCuePhase *phase asm("r6") =
                (const struct WeaponCuePhase *)((u8 *)record->phases + offset);
            Player_SetAnimation(p, Player_SelectActionCue(p), record->speed, 2);
            if ((++(p->timer)) > (phase->delay)) {
                SetHistorySentinel(p);
                Player_SetActionPhase(p, 3);
            }
            if ((p->inputKeys)[1] & 2)
                p->comboPending = 1;
            ClearContactTimer(p);
            break;
        }

        case 3: {
            register u32 offset asm("r0") = p->comboIndex * sizeof(struct WeaponCuePhase);
            register const struct WeaponCuePhase *phase =
                (const struct WeaponCuePhase *)((u8 *)record->phases + offset);
            if (!(p->timer)) {
                s32 cost = Player_UpdateStatThreshold(p);
                if ((p->currentStatB) <= cost)
                    p->currentStatB = 0;
                else
                    p->currentStatB -= cost;
            }
            ++(p->timer);
            p->contactParameter = 10;
            if ((p->inputKeys)[1] & 2)
                p->comboPending = 1;
            if (phase->forced)
                forced = 1;
            if (Player_SetAnimation(p, Player_SelectActionCue(p), phase->speed, 2) ||
                Player_IsActionCueReady(p)) {
                if ((p->comboPending) && Player_HasWeaponComboEntry(p)) {
                    p->comboPending = 0;
                    ++(p->comboIndex);
                    Player_SetActionPhase(p, 2);
                } else
                    Player_SetActionPhase(p, 4);
            }
            sub_0814D264(p, forced);
            break;
        }

        case 4: {
            register u32 offset asm("r0") = p->comboIndex * sizeof(struct WeaponCuePhase);
            register const struct WeaponCuePhase *phase asm("r6") =
                (const struct WeaponCuePhase *)((u8 *)record->phases + offset);
            ++(p->timer);
            Player_SetAnimation(p, Player_SelectActionCue(p), phase->speed, 2);
            if ((p->timer) > (phase->releaseDuration))
                Player_SetActionPhase(p, 5);
            break;
        }

        case 5: {
            u8 *pending = &p->comboPending;
            u32 zero = 0;
            *pending = zero;
            if (Player_SetAnimation(p, Player_SelectActionCue(p), record->speed, 2) ||
                ((p->timer) > 30)) {
                ActorSequence_Begin(p, 0, 0);
                p->comboIndex = zero;
            }
            break;
        }

        default:
            ActorSequence_Begin(p, 0, 0);
            break;
    }
}
