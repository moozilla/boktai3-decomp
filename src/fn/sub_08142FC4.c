#include "global.h"
#define B(n) (*(u8 *)(p + (n)))
#define H(n) (*(u16 *)(p + (n)))
#define W(n) (*(u32 *)(p + (n)))
s32 Player_ConsumeStateEntry(void *);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(void *, s32, s32, s32);
void Player_SetActionPhase(void *, u8);
void Player_SetActionOrigin(void *);
void ActorSequence_Begin(void *, s32, s32);
s32 Player_CanRecoverResourceWhileUnflagged(void *);
void Sound_PlaySong(u32);
void Sound_StopSong(u32);
void ConvergingEffects_EmitFarParticle(void *, void *);
static inline void OrFlags(u32 *a, u32 mask)
{
    *a |= mask;
}
static inline void SetPulse(u8 *p, u32 value)
{
    H(0xad8) = value;
}
static inline void SetHalf(u16 *a, u32 value)
{
    *a = value;
}
static inline void SetByte(u8 *a, u32 value)
{
    *a = value;
}
void Player_UpdateUnflaggedResourceRecovery(u8 *p)
{
    s16 k2 = 0;
    if (Player_ConsumeStateEntry(p)) {
        Player_SetActionPhase(p, 0);
    }
    switch (*((u8 *)(p + 0x458))) {
        case 0:
            Player_SetAnimation(p, ActorVariant_GetTableValue(B(0x418), 0), 64, 1);
            Player_SetActionPhase(p, 1);
            break;
        case 1:
            if (H(0x45a) == 0) {
                u32 zero = 0;
                if ((u8)(B(0x3a4) - 2) <= 4) {
                    B(0x3a4) = 4;
                    B(0x3a2) = 1;
                    B(0x3a3) = zero;
                } else {
                    B(0x3a4) = zero;
                    B(0x3a2) = zero;
                    B(0x3a3) = zero;
                }
                Player_SetActionOrigin(p);
                ++H(0x45a);
            }
            *((u16 *)(p + 0x6fc)) = 15;
            if (!Player_SetAnimation(p, ActorVariant_GetTableValue(*((u8 *)(p + 0x418)), 8), 64,
                                     1)) {
                break;
            }
            Sound_PlaySong(0xcb);
            if (!((*(*((u16 **)(p + 0x348)))) & 1)) {
                goto finish;
            }
            if (Player_CanRecoverResourceWhileUnflagged(p)) {
                Sound_PlaySong(0x2e4);
                B(0x4fd) = 1;
                H(0x4f8) = 0;
                Player_SetActionPhase(p, 2);
                break;
            }
            H(0x4f8) = 0;
            goto pause;
        case 2: {
            u32 zero;
            OrFlags((u32 *)(p + 0x20), 0xa0);
            if (H(0x4f8) <= 4) {
                if (++H(0x4f8) == 5) {
                    Sound_PlaySong(0x239);
                }
            }
            Player_SetAnimation(p, ActorVariant_GetTableValue(B(0x418), 9), 64, 1);
            H(0x6fc) = 30;
            Player_SetActionOrigin(p);
            ConvergingEffects_EmitFarParticle(p, p + 0xa80);
            SetHalf((u16 *)(p + 0xad6), 0x16f);
            {
                u32 counter = H(0x45a);
                if (counter <= 31) {
                    SetPulse(p, counter);
                } else {
                    SetPulse(p, 64 - counter);
                }
            }
            {
                u16 *timer = (u16 *)(p + 0x45a);
                u32 value = *timer + 1;
                zero = k2;
                *timer = value;
                if (((u16)value) > 64) {
                    *timer = 0;
                }
            }
            if (!(*(*(u16 **)(p + 0x348)) & 1)) {
                Sound_StopSong(0x239);
                B(0x4fd) = zero;
                goto finish;
            }
            if (Player_CanRecoverResourceWhileUnflagged(p)) {
                break;
            }
            Sound_StopSong(0x239);
            B(0x4fd) = zero;
        }
        pause:
            Player_SetActionPhase(p, 3);
            break;
        case 3:
            OrFlags((u32 *)(p + 0x20), 0x80);
            Player_SetAnimation(p, ActorVariant_GetTableValue(B(0x418), 10), 64, 1);
            H(0x6fc) = 30;
            if (!(*(*(u16 **)(p + 0x348)) & 1)) {
            finish:
                Player_SetActionPhase(p, 4);
                break;
            }
            if (!Player_CanRecoverResourceWhileUnflagged(p)) {
                break;
            }
            Sound_PlaySong(0x239);
            B(0x4fd) = 1;
            H(0x4f8) = 0;
            Player_SetActionPhase(p, 2);
            break;
        case 4:
            H(0x6fc) = 15;
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(B(0x418), 11), 64, 1)) {
                SetByte(p + 0x3a2, B(0x3a4));
                B(0x3a3) = 0;
                ActorSequence_Begin(p, 0, 0);
            }
            break;
    }
}
