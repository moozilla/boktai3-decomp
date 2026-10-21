// MATCHING_CONCESSION: register-pinning
#include "global.h"
#define gDirectionSineTable gUnk_086149C4

struct EffectPosition {
    s16 x, y, z, pad;
};
struct EffectActor {
    u8 pad0[0x54];
    s16 x, y, z;
    u8 pad5A[0x4F];
    u8 direction;
    u8 padAA[0x3DE];
    u8 effect[0x62];
    u8 mode;
    u8 pad4EB[0xD];
    s16 parameter;
};

extern const s16 gUnk_086149C4[];
void Projectile_InitializeParams(u8 *);
void Projectile_SetFlightParams(u8 *, u16, u16, u8, u32);
void Projectile_SetDamageParams(u8 *, u32, u32, u32, u32, u32);
void Projectile_SetCollisionSize(u8 *, u16, u16, u16);
void Projectile_SetCollisionMask(u8 *, u16);
void Projectile_SetTypeAndElement(u8 *, u32, u32);
s32 Projectile_CreateForOwner(u8 *, s16 *, u32);
static inline s32 ScaleOffset(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -(-value >> 12);
}
static inline void SetPosition(s16 *pos, s32 x, s32 y, s32 z)
{
    s32 zero;
    s16 *dst;
    y += 0xE6;
    zero = 0;
    dst = pos;
    dst[0] = x;
    pos[1] = y;
    pos[2] = z;
    pos[3] = zero;
}
static inline u8 DirectionAngle(struct EffectActor *actor)
{
    return ((actor->direction + 5) & 7) << 5;
}
void ActorEffect_InitializeOffset(struct EffectActor *p)
{
    struct EffectActor *actor = p;
    struct EffectPosition pos;
    s32 x;
    register s32 z asm("r2");
    u32 angle;
    Projectile_InitializeParams(p->effect);
    if (p->mode == 2)
        Projectile_SetTypeAndElement(p->effect, 2, 6);
    else
        Projectile_SetTypeAndElement(p->effect, 1, 6);
    angle = DirectionAngle(actor);
    Projectile_SetFlightParams(p->effect, 0, 0x3C, angle, 0x18);
    Projectile_SetDamageParams(p->effect, 0x10, 2, p->parameter, 0x32, 0x28);
    Projectile_SetCollisionSize(p->effect, 0x40, 0x64, 0x40);
    Projectile_SetCollisionMask(p->effect, 0x10);
    SetPosition((s16 *)&pos, actor->x, actor->y, actor->z);
    angle = DirectionAngle(actor);
    x = gUnk_086149C4[(angle + 0x40) & 0xFF];
    z = gUnk_086149C4[angle & 0xFF];
    x = ScaleOffset(0xB4 * x);
    pos.x = pos.x + x;
    z = ScaleOffset(0xB4 * z);
    pos.z = pos.z + z;
    Projectile_CreateForOwner(p->effect, (s16 *)&pos, 1);
}
