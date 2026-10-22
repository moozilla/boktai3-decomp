#include "global.h"
static inline s32 Scale12(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}

struct RecoilVec {
    s32 x, y, z, w;
};
struct RecoilVel {
    s16 x, y, z;
};
struct RecoilMsg {
    u8 pad0[8];
    s32 type;
    u8 padc[4];
    s16 x;
    u8 pad12[2];
    s16 z;
    u8 pad16[0xa];
    s16 dx;
    u8 pad22[2];
    s16 dz;
};
extern s16 gUnk_086149C4[];
u8 ScenePool_CanAcceptControl(u8 *);
s32 Math_Atan2(s32, s32);
static inline u8 Recoil_IsIgnored(s32 type)
{
    u8 r = 0;
    if (type == 2)
        r = 1;
    return r;
}
static inline void Recoil_SetVelocity(struct RecoilVel *out, s32 angle, s32 k)
{
    out->x = Scale12(gUnk_086149C4[(angle + 0x40) & 0xff] * k);
    out->y = 0;
    out->z = Scale12(gUnk_086149C4[angle & 0xff] * k);
}
void ActorRecoil_ApplyDirectionalImpulse(u8 *actor, struct RecoilMsg *msg)
{
    struct RecoilVec src, dst, diff;
    struct RecoilVel vel;
    s32 angle;
    s32 a, b, c, d;
    if (!ScenePool_CanAcceptControl(actor))
        return;
    if (Recoil_IsIgnored(msg->type))
        return;
    a = *(s16 *)(actor + 0x57C) + *(s16 *)(actor + 0x58C);
    dst.x = a;
    b = *(s16 *)(actor + 0x580) + *(s16 *)(actor + 0x590);
    dst.z = b;
    c = msg->x + msg->dx;
    src.x = c;
    d = msg->z + msg->dz;
    src.z = d;
    diff.x = a - c;
    diff.z = b - d;
    angle = Math_Atan2(a - c, b - d);
    angle = (angle >> 5) << 5;
    Recoil_SetVelocity(&vel, angle, 8);
    *(s32 *)(actor + 0x5CC) = vel.x << 10;
    *(s32 *)(actor + 0x5D4) = vel.z << 10;
    if ((u32)(angle - 10) > 0xEC) {
        *(s32 *)(actor + 0x5AC) += 0x3000;
        actor[0x5EC] = 10;
    } else {
        *(s32 *)(actor + 0x5AC) -= 0x2000;
        *(s32 *)(actor + 0xC0) = 0;
        actor[0x5EC] = 15;
    }
}
