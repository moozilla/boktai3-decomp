// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "engine_globals.h"
#include "ram.h"
// confidence=low: computes a target-relative endpoint before accelerating the turn.

struct Orbit {
    u8 pad[0x134];
    u16 endX, endY, endZ;
    u8 pad13a[2];
    u16 startX, startY, startZ;
    u8 pad142[2];
    u16 dx, dy, dz;
};
extern s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);
s32 Div(s32, s32);
void ActorPhase_AccelerateTurn(void);
void Sword_BeginTargetOrbit(u8 *p)
{
    u8 *owner = *(u8 **)(p + 0x160);
    u8 *target = *(u8 **)(owner + 0x60);
    s32 angle;
    struct Orbit *o = (void *)p;
    angle = Math_Atan2(*(s16 *)(target + 0x30) - *(s16 *)(p + 0x1c),
                       *(s16 *)(target + 0x34) - *(s16 *)(p + 0x20));
    p[0x11a] = angle;
    {
        s32 rotation = 0xa0 - angle;
        p[0x188] = rotation;
    }
    gRandomIndex = (gRandomIndex + 1) & 1023;
    {
        register s32 turn asm("r1");
        s32 choice;
        u32 one;
        {
            u32 offset = gRandomIndex * 2;
            register const u16 *table asm("r3") = gRandomTable;
            choice = *(const u16 *)(offset + (u32)table);
        }
        one = 1;
        choice &= one;
        {
            register s32 negative asm("r3") = -1;
            asm("" : "+r"(negative));
            turn = negative;
        }
        if (choice)
            turn = 1;
        p[0x18a] = turn;
        p[0x18b] = one;
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
