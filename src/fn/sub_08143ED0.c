#include "global.h"

// confidence=low: updates movement and step cues while constraining the tether target.
#ifdef NONMATCHING
// fuzzy: 99.9%
// rtlprobe.py canon: CSE_NOCANON=265 (FlagFour's return register) matches, as in sub_08141C40.
#define B(n) (*(u8 *)(p + (n)))
#define H(n) (*(u16 *)(p + (n)))
#define S(n) (*(s16 *)(p + (n)))
#define W(n) (*(u32 *)(p + (n)))
extern u8 *gGameState;
extern const s16 gUnk_086149C4[];
s32 Player_ConsumeStateEntry(void *);
void ActorSequence_Begin(void *, s32, s32);
s32 ActorVariant_GetTableValue(u32, u32);
s32 Player_CalculateMovementSpeed(void *);
u32 Player_GetSmoothedDirection(void *);
void Player_UpdateFacingSide(void *);
u32 RepeatedEffect_RequestNearby(void *);
u32 Player_IsNormalActionAllowed(u32);
void Player_PlayPositionEffect(void *, void *, s32, u32, void *, u32, u32);
void Sound_PlaySong(u32);
s32 Player_SetAnimation(void *, s32, s32, s32);
void Player_ConstrainTetherTarget(void *, s32, s32, s32);
static inline s32 scale12(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
static inline s32 scale6(s32 v)
{
    if (v >= 0)
        return v >> 6;
    return -((-v) >> 6);
}
static inline u8 modeFive(u8 *p)
{
    u16 active = H(0x538);
    u8 result = 0;
    if (active && H(0x534) == 5)
        result = 1;
    return result;
}
static inline u32 flags(u32 *p, u32 mask)
{
    return *p & mask;
}
static inline u32 FlagFour(u8 *p)
{
    if (!p)
        return 0;
    return flags((u32 *)(p + 0x448), 4);
}
static inline u32 FlagMovement(u8 *p)
{
    if (!p)
        return 0;
    return flags((u32 *)(p + 0x448), 0x20000);
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
void Player_UpdateTetherMovement(u8 *p)
{
    s32 animation, rate, moving, cosine, sine, angle, speed;
    if (Player_ConsumeStateEntry(p))
        ActorSequence_Begin(p, 6, 0);
    W(0x20) &= ~1;
    if (S(0x34C) < 0 || S(0x4B6) != 0) {
        animation = ActorVariant_GetTableValue(B(0x418), 16);
        if (B(0x48C))
            angle = ((B(0x48F) + 1) & 7) << 5;
        else
            angle = ((B(0x3A4) + 1) & 7) << 5;
        cosine = gUnk_086149C4[(angle + 64) & 255];
        sine = gUnk_086149C4[angle & 255];
        moving = 0;
        rate = 64;
    } else {
        animation = ActorVariant_GetTableValue(B(0x418), 17);
        speed = Player_CalculateMovementSpeed(p);
        moving = 1;
        B(0x3A4) = Player_GetSmoothedDirection(p);
        Player_UpdateFacingSide(p);
        if (B(0x48C)) {
            rate = 32;
            angle = ((B(0x48F) + 1) & 7) << 5;
            cosine = gUnk_086149C4[(angle + 64) & 255];
            sine = gUnk_086149C4[angle & 255];
        } else {
            if (speed == 8)
                rate = 64;
            else if (speed < 8)
                rate = 64 + ((8 - speed) << 3);
            else
                rate = 64 - ((speed - 8) << 2);
            angle = ((B(0x3A4) + 1) & 7) << 5;
            cosine = gUnk_086149C4[(angle + 64) & 255];
            sine = gUnk_086149C4[angle & 255];
            if (H(0xA0) & 3) {
                H(0x38) = -scale12(cosine * speed);
                H(0x3C) = -scale12(sine * speed);
                if (B(0x28F) == 1) {
                    if (S(0x3C) < 0)
                        H(0x3C) = scale6(S(0x3C) * 40);
                    else
                        H(0x3C) = scale6(S(0x3C) * 52);
                } else if (B(0x28F) == 2) {
                    if (S(0x38) < 0)
                        H(0x38) = scale6(S(0x38) * 40);
                    else
                        H(0x38) = scale6(S(0x38) * 52);
                }
            }
        }
    }
    if (moving && S(0x34E) >= 0 && !(H(0xA0) & 3) && !H(0x98)) {
        if (p && H(0x538) && H(0x534) == 0)
            goto animate;
        if (FlagFour(p))
            goto animate;
        if (FlagMovement(p)) {
            if (!H(0xA0))
                Player_PlayPositionEffect(p, p + 0x30, 1, 1, p + 0x30, 0, 0x5AE);
            else
                Player_PlayPositionEffect(p, p + 0x30, 1, 1, p + 0x30, 0, 0x5AD);
        } else if (RepeatedEffect_RequestNearby(p + 0x30)) {
            if (!H(0xA0))
                Player_PlayPositionEffect(p, p + 0x30, 1, 1, p + 0x30, 0, 0x163);
            else
                Player_PlayPositionEffect(p, p + 0x30, 1, 1, p + 0x30, 0, 0x162);
        } else if (Player_IsNormalActionAllowed(H(0x292)) && !(p && modeFive(p))) {
            if (!H(0xA0))
                Player_PlayPositionEffect(p, p + 0x30, 1, 1, p + 0x30, 0, 0x194);
            else
                Player_PlayPositionEffect(p, p + 0x30, 1, 1, p + 0x30, 0, 0x193);
        } else {
            if (H(0x474) || !B(0x28F)) {
                if (!H(0xA0))
                    Sound_PlaySong(0xCA);
                else
                    Sound_PlaySong(0xC9);
            } else {
                if (!H(0xA0))
                    Sound_PlaySong(0x28E);
                else
                    Sound_PlaySong(0x28D);
            }
            if (HasGlobalFlag())
                Player_PlayPositionEffect(p, p + 0x30, 1, 2, 0, 0, 0);
        }
    }
animate:
    Player_SetAnimation(p, animation, rate, 1);
    if (moving || B(0x48C))
        Player_ConstrainTetherTarget(p, cosine, sine, 150);
}

#else
INCLUDE_ASM("asm/nonmatching", Player_UpdateTetherMovement);
#endif
