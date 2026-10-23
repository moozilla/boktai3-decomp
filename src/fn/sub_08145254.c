#include "global.h"

// confidence=low: interpolates from the saved origin to a scripted endpoint.
#define Player_UpdateInterpolatedMotion sub_08145254
#define H(n)                            (*(u16 *)(p + (n)))
#define S(n)                            (*(s16 *)(p + (n)))
struct MotionPosition {
    s16 x, y, z, pad;
};
s32 Player_ConsumeStateEntry(void *);
void Player_AddControlFlags(void *, u32);
s32 Div(s32, s32);
s32 ActorVariant_GetTableValue(u32, u32);
s32 Player_SetAnimation(void *, s32, s32, s32);
static inline void OrFlags(u32 *p, u32 mask)
{
    *p |= mask;
}
static inline s32 Scale8(s32 value)
{
    if (value >= 0)
        return value >> 8;
    return -((-value) >> 8);
}
static inline s32 ScaleStep(s32 value)
{
    s32 result = Scale8(value);
    return result;
}
static inline void SetMotionSize(u8 *p)
{
    u16 *dst = (u16 *)(p + 0xAD6);
    u32 value;

    value = 500;

    *dst = value;
}
void Player_UpdateInterpolatedMotion(u8 *p)
{
    if (Player_ConsumeStateEntry(p))
        *(struct MotionPosition *)(p + 0xA8C) = *(struct MotionPosition *)(p + 0x30);
    SetMotionSize(p);
    H(0xAD8) = 32;
    OrFlags((u32 *)(p + 0x20), 0x1100);
    Player_AddControlFlags(p, 8);
    {
        u16 *duration = (u16 *)(p + 0xA9C);
        u16 *elapsed = (u16 *)(p + 0x45A);
        u32 durationValue = *duration;
        if (durationValue <= *elapsed) {
            *(struct MotionPosition *)(p + 0x30) = *(struct MotionPosition *)(p + 0xA94);
        } else {
            s32 dx = (S(0xA94) - S(0xA8C)) << 8;
            s32 dy = (S(0xA96) - S(0xA8E)) << 8;
            s32 dz = (S(0xA98) - S(0xA90)) << 8;
            s32 zstep;

            if (!durationValue)
                *duration = 1;
            dx = Div(dx, *duration);

            dy = Div(dy, *duration);

            dz = Div(dz, *duration);

            dx = ScaleStep(dx * *elapsed);

            dy = ScaleStep(dy * H(0x45A));

            zstep = Scale8(dz * H(0x45A));
            H(0x30) = H(0xA8C) + dx;
            H(0x32) = H(0xA8E) + dy;
            H(0x34) = H(0xA90) + zstep;
        }
    }
    Player_SetAnimation(p, ActorVariant_GetTableValue(p[0x418], 49), 64, 2);
    ++H(0x45A);
}
