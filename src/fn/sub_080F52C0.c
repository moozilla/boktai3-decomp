// MATCHING_CONCESSION: register-pinning
#include "global.h"
// confidence=low: computes a target-relative endpoint before accelerating the turn.
#define Sword_BeginTargetOrbit sub_080F52C0

#ifdef NONMATCHING
// fuzzy: 95.5%
struct Orbit {
    u8 pad[0x134];
    u16 endX, endY, endZ;
    u8 pad13a[2];
    u16 startX, startY, startZ;
    u8 pad142[2];
    u16 dx, dy, dz;
};
extern u32 gUnk_03005308;
extern u16 gUnk_0203B400[];
extern s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);
s32 Div(s32, s32);
void ActorPhase_AccelerateTurn(void);
void Sword_BeginTargetOrbit(u8 *p)
{
    u8 *owner = *(u8 **)(p + 0x160);
    u8 *target = *(u8 **)(owner + 0x60);
    s32 direction, angle;
    struct Orbit *o = (void *)p;
    angle = Math_Atan2(*(s16 *)(target + 0x30) - *(s16 *)(p + 0x1c),
                       *(s16 *)(target + 0x34) - *(s16 *)(p + 0x20));
    p[0x11a] = angle;
    {
        s32 rotation = 0xa0 - angle;
        asm("" : "+r"(rotation));
        p[0x188] = rotation;
    }
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    {
        s32 bits = *(u16 *)((gUnk_03005308 << 1) + (u32)gUnk_0203B400);
        register s32 mask asm("r2") = 1;
        asm("" : "+r"(mask));
        bits &= mask;
        direction = -1;
        if (bits)
            direction = 1;
        p[0x18a] = direction;
        p[0x18b] = mask;
    }

    o->startX = *(u16 *)(p + 0x1c);
    o->startY = *(u16 *)(p + 0x1e);
    o->startZ = *(u16 *)(p + 0x20);
    o->endX = *(u16 *)(p + 0x1c) + Div(gUnk_086149C4[(p[0x11a] + 64) & 255] * 800, 4096);
    o->endY = *(u16 *)(*(u8 **)(owner + 0x60) + 0x32) + 220;
    o->endZ = *(u16 *)(p + 0x20) + Div(gUnk_086149C4[p[0x11a]] * 800, 4096);
    o->dx = o->endX - o->startX;
    o->dy = o->endY - o->startY;
    o->dz = o->endZ - o->startZ;
    *(void (**)(void))(p + 0x178) = ActorPhase_AccelerateTurn;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_080F52C0);
#endif
