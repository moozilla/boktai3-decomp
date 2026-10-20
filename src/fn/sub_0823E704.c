#include "global.h"
#ifdef NONMATCHING
// fuzzy: 82.2%
// remaining: agbcc CSEs the constant 7 into a callee-saved reg (orig reloads it), shifting resource/flags registers
struct PairE7 {
    u32 a, b;
};
u32 Player_GetConfiguredActionPower(u8 *);
u32 Player_CollectCallbackFlags(u8 *);
void Player_ConsumeActionResource(u8 *);
void Collider_SetAttackProperties(u8 *, u32, u32, u32, u32, u32);
void sub_0823E704(u8 *p)
{
    u8 *q = p + 0xb44;
    u32 resource, flags;
    *(struct PairE7 *)(p + 0xbe8) = *(struct PairE7 *)(p + 0x30);
    *(u16 *)(p + 0xbea) += 230;
    {
        u32 v = ((p[0x3a4] + 5) & 7) << 5;
        p[0xc0c] = v;
    }
    resource = Player_GetConfiguredActionPower(p);
    flags = Player_CollectCallbackFlags(p);
    if (p[0x418] == 1) {
        *(u16 *)(p + 0xc00) = 29;
        *(u16 *)(p + 0xc08) = 7;
        *(u16 *)(p + 0xc04) = 4;
        *(u16 *)(p + 0xc06) = 6;
        flags |= 0x40000;
    } else {
        *(u16 *)(p + 0xc00) = 19;
        *(u16 *)(p + 0xc08) = 11;
        *(u16 *)(p + 0xc04) = 6;
        *(u16 *)(p + 0xc06) = 9;
    }
    {
        s32 bit = *(s8 *)(q + 0xc9);
        if (bit >= 0) {
            flags |= 1U << bit;
            Player_ConsumeActionResource(p);
        }
    }
    Collider_SetAttackProperties(q + 0x48, resource, *(u16 *)(q + 0xb8), 4, flags,
                                 *(u16 *)(q + 0xba));
    {
        u8 h = q[0xc8];
        u32 z = 0;
        q[0x8a] = h;
        *(u16 *)(q + 0xc6) = z;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0823E704);
#endif
