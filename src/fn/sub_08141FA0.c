#include "global.h"
// confidence=low: spends shared resource to enter a timed fast movement phase.
#define B(n) (*(u8 *)(p + (n)))
#define H(n) (*(u16 *)(p + (n)))
#define S(n) (*(s16 *)(p + (n)))
extern u8 *gGameState;
s32 Player_ConsumeStateEntry(void *);
s32 Player_CanSpendSharedResource(void *, s32);
void Sound_PlaySong(u32);
s32 Div(s32, s32);
void Player_SetActionPhase(void *, u8);
void ActorSequence_Begin(void *, s32, s32);
s32 Player_GetSmoothedDirection(void *);
void Player_SetDirectionalVelocity(void *, s32);
void Player_EmitMovementEffects(void *, s32);
void Player_UpdateFacingSide(void *);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(void *, s32, s32, s32);
static inline u32 Mask(u32 *f, u32 m)
{
    return *f & m;
}
static inline void SetByte(u8 *a, u32 v)
{
    *a = v;
}
static inline void SetHalf(u16 *a, u32 v)
{
    *a = v;
}
static inline u32 HasDashFlag(u8 *p)
{
    if (!p)
        return 0;
    return Mask((u32 *)(p + 0x448), 0x20);
}
void Player_UpdateResourceDash(u8 *p)
{
    if (Player_ConsumeStateEntry(p)) {
        if (Player_CanSpendSharedResource(p, 1) || HasDashFlag(p)) {
            Sound_PlaySong(0x392);
            if (HasDashFlag(p))
                B(0x4f0) = 26;
            else {
                s32 value = *(s16 *)(gGameState + 0x876) * 2 + 14;
                SetByte(p + 0x4f0, value);
            }
            if (B(0x4f0) > 32)
                B(0x4f0) = 32;
            B(0x4f1) = Div(512, B(0x4f0));
            Player_SetActionPhase(p, 0);
        } else {
            Sound_PlaySong(0x192);
            Player_SetActionPhase(p, 1);
        }
    }
    switch (B(0x458)) {
        case 0:
            if (++H(0x45a) >= B(0x4f1)) {
                ActorSequence_Begin(p, 0, 0);
                break;
            }
            if (H(0x45a) < B(0x4f1) - 8) {
                SetHalf((u16 *)(p + 0xad6), 0x172);
                H(0xad8) = 32;
            }
            if (S(0x34c) != -1 && S(0x4b6) == 0)
                B(0x3a4) = Player_GetSmoothedDirection(p);
            Player_SetDirectionalVelocity(p, B(0x4f0));
            Player_EmitMovementEffects(p, 1);
            Player_UpdateFacingSide(p);
            Player_SetAnimation(p, ActorVariant_GetTableValue(B(0x418), 1), 32, 1);
            break;
        case 1:
            Player_SetAnimation(p, ActorVariant_GetTableValue(B(0x418), 0), 64, 1);
            if (++H(0x45a) > 7)
                ActorSequence_Begin(p, 0, 0);
            break;
    }
}
