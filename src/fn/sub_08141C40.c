#include "global.h"

#ifdef NONMATCHING
// fuzzy: 99.9%
// The first null-player branch targets the next null check rather than the shared effect path.
// rtlprobe.py canon: CSE_NOCANON=51 (FlagFour's return register) matches; CSE makes it canonical
// because its `return 0` set precedes the block that copies Mask's value into it.
#define B(n) (*(u8 *)(p + (n)))
#define H(n) (*(u16 *)(p + (n)))
extern u8 *gGameState;
void Player_PlayPositionEffect(void *, void *, u32, u32, void *, u32, u32);
s32 RepeatedEffect_RequestNearby(void *);
s32 Player_IsNormalActionAllowed(u32);
void Sound_PlaySong(u32);
static inline u32 Mask(u32 *v, u32 m)
{
    return *v & m;
}
static inline u32 HasFlagValue(u8 *p, u32 m)
{
    if (!p)
        return 0;
    return Mask((u32 *)(p + 0x448), m);
}
static inline u32 FlagFour(u8 *p)
{
    if (!p)
        return 0;
    return Mask((u32 *)(p + 0x448), 4);
}
static inline u32 FlagMovement(u8 *p)
{
    if (!p)
        return 0;
    return Mask((u32 *)(p + 0x448), 0x20000);
}
static inline u8 IsMode(u8 *p, u32 mode)
{
    u16 active = H(0x538);
    u8 result = 0;
    if (active && H(0x534) == mode)
        result = 1;
    return result;
}
static inline u32 GlobalMask(u32 mask)
{
    return *(u32 *)(gGameState + 0x868) & mask;
}
static inline u8 HasGlobalFlag(void)
{
    if (GlobalMask(1))
        return 1;
    return 0;
}
// MATCHING_CONCESSION: register-pinning
void Player_EmitMovementEffects(u8 *p, s32 quiet)
{
    register u8 *pos asm("r4");
    if ((H(0xa0) & 3) || H(0x98))
        return;
    if (p && H(0x538) && H(0x534) == 0)
        return;
    if (FlagFour(p))
        return;
    if (FlagMovement(p)) {
        if (H(0xa0) == 0) {
            pos = p + 0x30;
            Player_PlayPositionEffect(p, pos, 1, 1, pos, 0, 0x5ae);
        } else {
            pos = p + 0x30;
            Player_PlayPositionEffect(p, pos, 1, 1, pos, 0, 0x5ad);
        }
        RepeatedEffect_RequestNearby(pos);
        return;
    }
    pos = p + 0x30;
    if (RepeatedEffect_RequestNearby(pos)) {
        if (H(0xa0) == 0)
            Player_PlayPositionEffect(p, pos, 1, 1, pos, 0, 0x163);
        else
            Player_PlayPositionEffect(p, pos, 1, 1, pos, 0, 0x162);
        return;
    }
    if (Player_IsNormalActionAllowed(H(0x292)) && !(p && IsMode(p, 5))) {
        if (H(0xa0) == 0)
            Player_PlayPositionEffect(p, pos, 1, 1, pos, 0, 0x194);
        else
            Player_PlayPositionEffect(p, pos, 1, 1, pos, 0, 0x193);
        return;
    }
    if (!quiet && !FlagMovement(p)) {
        if (p && IsMode(p, 5)) {
            if (H(0xa0) == 0)
                Sound_PlaySong(0xc8);
            else
                Sound_PlaySong(0xc7);
        } else if (H(0x474) || !B(0x28f)) {
            if (H(0xa0) == 0)
                Sound_PlaySong(0xca);
            else
                Sound_PlaySong(0xc9);
        } else {
            if (H(0xa0) == 0)
                Sound_PlaySong(0x28e);
            else
                Sound_PlaySong(0x28d);
        }
    }
    if (HasGlobalFlag())
        Player_PlayPositionEffect(p, pos, 1, 2, 0, 0, 0);
}

#else
INCLUDE_ASM("asm/nonmatching", Player_EmitMovementEffects);
#endif
