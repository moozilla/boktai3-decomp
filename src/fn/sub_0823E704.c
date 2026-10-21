#include "global.h"
// MATCHING_CONCESSION: register-pinning
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
        u32 x = p[0x3a4] + 5;
        u32 m = 7;
        u32 v;
        asm("" : "+r"(m));
        v = (x & m) << 5;
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
        s8 *bp = (s8 *)(q + 0xc9);
        if (*bp >= 0) {
            flags |= 1U << *bp;
            Player_ConsumeActionResource(p);
        }
    }
    Collider_SetAttackProperties(q + 0x48, resource, *(u16 *)(q + 0xb8), 4, flags,
                                 *(u16 *)(q + 0xba));
    {
        u8 h = q[0xc8];
        u8 *d = q + 0x8a;
        u16 z = 0;
        *d = h;
        *(u16 *)(q + 0xc6) = z;
    }
}
