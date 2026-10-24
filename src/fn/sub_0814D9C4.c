#include "global.h"
#include "player.h"

#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Remaining differences: register allocation; reviewed body has no structural differences.
// MATCHING_CONCESSION: register-pinning
// confidence=low: table-driven weapon cue and combo phases drive the attack collider.
#define Player_UpdateWeaponCueCombo sub_0814D9C4
#define H(n)                        (*(u16 *)(p + (n)))
#define B(n)                        (*(p + (n)))
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
static inline void SetHistorySentinel(u8 *p)
{
    u16 *field = (u16 *)(p + 0xC1A);
    u32 missing = 0xFFFF;
    *field = missing;
}
static inline void ClearContactTimer(u8 *p)
{
    u16 *timer = (u16 *)(p + 0xC0A);
    u32 zero = 0;
    *timer = zero;
}
void Player_UpdateWeaponCueCombo(u8 *p)
{
    struct PlayerActionCueInfo info;
    register const u8 *record asm("r4");
    s32 forced = 0;
    typeof(record) t1;
    ActionRecord_CopyFromEntry(*((void **)(p + 0xBF0)), &info);
    record = (t1 = (const void *)(gUnk_08E5FA6C + (info.cueIndex << 6)));
    asm("" : : "r"(t1));
    Player_AddControlFlags(p, 0x100);
    switch (*(p + 0x458)) {
        case 0: {
            u16 *timer = &(*((u16 *)(p + 0x45A)));
            if (!(*timer))
                Player_SelectActionVariant(p);
            if (Player_SetAnimation(p, Player_SelectActionCue(p), *((const u16 *)record), 2)) {
                *((u16 *)(p + 0xAAE)) = 0;
                Player_SetActionPhase(p, 1);
            }
            ++(*timer);
            FrameDisplay_UpdateSprite(*((void **)(((u8 *)p) + 0xC84)));
            break;
        }

        case 1:
            Player_UpdateAttackFacing(p);
            Player_SetAnimation(p, Player_SelectActionCue(p), *((const u16 *)record), 2);
            if (!((*(*((u16 **)(p + 0x348)))) & 2)) {
                Player_InitializeActionContact(p);
                Player_SetActionPhase(p, 2);
            } else
                ++(*((u16 *)(p + 0x45A)));
            break;

        case 2: {
            u32 offset = ((*((u16 *)(p + 0xAAE))) * 12) + 4;
            const u8 *phase = record + offset;
            Player_SetAnimation(p, Player_SelectActionCue(p), *((const u16 *)record), 2);
            if ((++(*((u16 *)(p + 0x45A)))) > (*((const u16 *)(phase + 4)))) {
                SetHistorySentinel(p);
                Player_SetActionPhase(p, 3);
            }
            if ((*((u16 **)(p + 0x348)))[1] & 2)
                *(p + 0xBF6) = 1;
            ClearContactTimer(p);
            break;
        }

        case 3: {
            u32 offset = ((*((u16 *)(p + 0xAAE))) * 12) + 4;
            const u8 *phase = record + offset;
            if (!(*((u16 *)(p + 0x45A)))) {
                s32 cost = Player_UpdateStatThreshold(p);
                if ((*((u16 *)(p + 0x428))) <= cost)
                    *((u16 *)(p + 0x428)) = 0;
                else
                    *((u16 *)(p + 0x428)) -= cost;
            }
            ++(*((u16 *)(p + 0x45A)));
            *((u16 *)(p + 0xBFE)) = 10;
            if ((*((u16 **)(p + 0x348)))[1] & 2)
                *(p + 0xBF6) = 1;
            if (phase[1])
                forced = 1;
            if (Player_SetAnimation(p, Player_SelectActionCue(p), *((const u16 *)(phase + 6)), 2) ||
                Player_IsActionCueReady(p)) {
                if ((*(p + 0xBF6)) && Player_HasWeaponComboEntry(p)) {
                    *(p + 0xBF6) = 0;
                    ++(*((u16 *)(p + 0xAAE)));
                    Player_SetActionPhase(p, 2);
                } else
                    Player_SetActionPhase(p, 4);
            }
            sub_0814D264(p, forced);
            break;
        }

        case 4: {
            u32 offset = ((*((u16 *)(p + 0xAAE))) * 12) + 4;
            const u8 *phase = record + offset;
            ++(*((u16 *)(p + 0x45A)));
            Player_SetAnimation(p, Player_SelectActionCue(p), *((const u16 *)(phase + 6)), 2);
            if ((*((u16 *)(p + 0x45A))) > (*((const u16 *)(phase + 10))))
                Player_SetActionPhase(p, 5);
            break;
        }

        case 5: {
            u8 *pending = p + 0xBF6;
            u32 zero = 0;
            *pending = zero;
            if (Player_SetAnimation(p, Player_SelectActionCue(p), *((const u16 *)record), 2) ||
                ((*((u16 *)(p + 0x45A))) > 30)) {
                ActorSequence_Begin(p, 0, 0);
                *((u16 *)(p + 0xAAE)) = zero;
            }
            break;
        }

        default:
            ActorSequence_Begin(p, 0, 0);
            break;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_0814D9C4);
#endif
