#include "global.h"
void ExplosionEffect_InitSprite(u8 *, u8 *, u8 *, s32);
void ExplosionEffect_InitCollider(u8 *, u8 *, u32, s32, s32, s32, s32);
void Collider_SetPosition(u8 *, u8 *, u32);
static inline void init(u16 *dest, u32 zero, u32 value)
{
    *dest++ = value;
    *dest = zero;
}
s32 ExplosionEffect_Init(u8 *p, u8 *pos, u32 flag, s32 duration, s32 arg4, s32 arg5, s32 arg6, s32 frame,
                 u32 arg8)
{
    u8 *emitter;
    ExplosionEffect_InitSprite(p + 0x18, p + 0x44, pos, frame);
    emitter = p + 0x60;
    ExplosionEffect_InitCollider(p, emitter, flag, duration, arg4, arg5, arg6);
    Collider_SetPosition(emitter, pos, 0);
    init((u16 *)(p + 0xb4), 0, arg8);
    return 0;
}
