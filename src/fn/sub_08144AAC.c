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
s32 Player_CanRecoverResourceWhileFlagged(void *);
void Sound_PlaySong(u32);
void Sound_StopSong(u32);
void ConvergingEffects_EmitNearParticle(void *, void *);
void Player_OnRecoveryActionEnd(void *);
void Player_SetPendingCallback(void *, void (*)(void *));
s32 PlayerBarrier_TryActivate(void *);
void PlayerBarrier_BeginRecovery(void *);
static inline void OrFlags(u32 *a, u32 mask)
{
    *a |= mask;
}
static inline void SetByte(u8 *a, u32 value)
{
    *a = value;
}
// confidence=low: refreshes the barrier through the resource recovery phases.
#define Player_UpdateBarrierRecovery sub_08144AAC
void Player_UpdateBarrierRecovery(u8 *p)
{
    if (Player_ConsumeStateEntry(p)) {
        Player_SetPendingCallback(p, Player_OnRecoveryActionEnd);
    }
    switch (*((u8 *)(p + 0x458))) {
        case 0:
            PlayerBarrier_TryActivate(*(void **)(p + 0xB40));
            Sound_PlaySong(0x128);
            Player_SetAnimation(p, ActorVariant_GetTableValue(B(0x418), 0), 64, 1);
            Player_SetActionPhase(p, 1);
            break;
        case 1:
            PlayerBarrier_TryActivate(*(void **)(p + 0xB40));
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
            if (Player_CanRecoverResourceWhileFlagged(p)) {
                B(0x4fd) = 1;
                H(0x4f8) = 0;
                Player_SetActionPhase(p, 2);
                break;
            }
            H(0x4f8) = 0;
            Player_SetActionPhase(p, 3);
            break;
        case 2:
            PlayerBarrier_TryActivate(*(void **)(p + 0xB40));
            Player_SetAnimation(p, ActorVariant_GetTableValue(B(0x418), 9), 64, 1);
            H(0x6FC) = 30;
            if (*(*(u16 **)(p + 0x348)) & 1)
                break;
            goto finish;
        case 3:
            if (PlayerBarrier_TryActivate(*(void **)(p + 0xB40)) == 1 && H(0x4F8) <= 19) {
                if (++H(0x4F8) > 19)
                    Sound_PlaySong(0xF5);
            }
            OrFlags((u32 *)(p + 0x20), 0x80);
            Player_SetAnimation(p, ActorVariant_GetTableValue(B(0x418), 10), 64, 1);
            H(0x6fc) = 30;
            if (!(*(*(u16 **)(p + 0x348)) & 1)) {
            finish:
                Player_SetActionPhase(p, 4);
                break;
            }
            if (!Player_CanRecoverResourceWhileFlagged(p)) {
                break;
            }
            Sound_PlaySong(0xd8);
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
                PlayerBarrier_BeginRecovery(*(void **)(p + 0xB40));
            }
            break;
    }
}
