#define ActorRecoil_ApplyDirectionalImpulse sub_08116538
#define ActorRecoil_CanApply                sub_0811628C
#define gRecoilSineTable                    gUnk_086149C4
// Tentative family and semantic names; layouts follow observed field offsets.
#include "global.h"
struct RecoilActor {
    u8 unknown0[0xc0];
    s32 fieldC0;
    u8 unknownC4[0x57c - 0xc4];
    s16 x;
    u8 unknown57E[2];
    s16 z;
    u8 unknown582[0xa];
    s16 offsetX;
    u8 unknown58E[2];
    s16 offsetZ;
    u8 unknown592[0x1a];
    s32 impulse;
    u8 unknown5B0[0x1c];
    s32 velocityX;
    s32 velocityY;
    s32 velocityZ;
    u8 unknown5D8[0x14];
    u8 state;
};
struct RecoilSource {
    u8 unknown[8];
    s32 kind;
    u8 unknownC[4];
    s16 x;
    u8 unknown12[2];
    s16 z;
    u8 unknown16[0xa];
    s16 offsetX;
    u8 unknown22[2];
    s16 offsetZ;
};
struct RecoilPoint {
    s32 x : 32;
    s32 y : 32;
    s32 z : 32;
    s32 w : 32;
};
extern const s16 gUnk_086149C4[];
u8 sub_0811628C(struct RecoilActor *, struct RecoilSource *);
s32 Math_Atan2(s32, s32);
static inline u8 RecoilSource_IsExcluded(struct RecoilSource *p)
{
    s32 kind = p->kind;
    u8 result = 0;
    if (kind == 2)
        result = 1;
    return result;
}
static inline void Recoil_MakeVelocity(s16 *out, s32 angle, s32 speed)
{
    s32 value = gUnk_086149C4[(angle + 64) & 255] * speed;
    s32 x;
    if (value >= 0)
        x = value >> 12;
    else
        x = -((-value) >> 12);
    out[0] = x;
    out[1] = 0;
    value = speed * gUnk_086149C4[angle & 255];
    if (value >= 0)
        out[2] = value >> 12;
    else
        out[2] = -((-value) >> 12);
}
void sub_08116538(struct RecoilActor *p, struct RecoilSource *source)
{
    struct RecoilPoint b, a, delta;
    s16 velocity[3];
    s32 angle, value;
    if (!sub_0811628C(p, source))
        return;
    if (RecoilSource_IsExcluded(source))
        return;
    a.x = p->x + p->offsetX;
    a.z = p->z + p->offsetZ;
    b.x = source->x + source->offsetX;
    b.z = source->z + source->offsetZ;
    delta.x = a.x - b.x;
    delta.z = a.z - b.z;
    angle = Math_Atan2(delta.x, delta.z);
    angle = (angle >> 5) << 5;
    Recoil_MakeVelocity(velocity, angle, 8);
    p->velocityX = velocity[0] << 10;
    p->velocityZ = velocity[2] << 10;
    if ((u32)(angle - 10) > 236) {
        p->impulse += 0x3000;
        p->state = 10;
    } else {
        p->impulse -= 0x2000;
        p->fieldC0 = 0;
        p->state = 15;
    }
}
