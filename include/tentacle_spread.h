#ifndef GUARD_TENTACLE_SPREAD_H
#define GUARD_TENTACLE_SPREAD_H

#include "scene_actor_reaction.h"

extern const s16 gUnk_086149C4[];
s32 Div(s32, s32);
void sub_080A9DB0(void *, s32, s32);
struct TentacleSegment {
    u16 posX;
    u8 pad2[2];
    u16 posZ;
    u8 pad6[0xE9 - 6];
    u8 flags;
    u8 padEA[0x100 - 0xEA];
    u16 offsetX, offsetY, offsetZ;
    u8 pad106[0x22];
};
struct TentacleBodyView {
    u8 pad0[0x3F0];
    struct TentacleSegment segments[3];
    u8 pad768[0x875 - 0x768];
    u8 spin;
    u8 pad876[0x8DA - 0x876];
    s16 segmentCount;
};
static inline u8 TentacleSpread_Take(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}
static inline u8 TentacleSpread_Test(struct TentacleSegment *s, u32 m)
{
    if (s->flags & m)
        return 1;
    return 0;
}
static inline s32 TentacleSpread_Shift(s32 n, u32 bits)
{
    if (n >= 0)
        return n >> bits;
    return -((-n) >> bits);
}
static inline u32 TentacleSpread_Scale(u8 spin)
{
    u32 magnitude = 0x7F;
    magnitude &= spin;
    return magnitude >> 4;
}

#endif
