#ifndef GUARD_TENTACLE_SPREAD_H
#define GUARD_TENTACLE_SPREAD_H

#include "scene_actor_reaction.h"
#include "tentacle_segment_animation.h"

extern const s16 gUnk_086149C4[];
s32 Div(s32, s32);
void sub_080A9DB0(void *, s32, s32);
static inline u8 TentacleSpread_Take(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}
static inline u8 TentacleSpread_Test(struct TentacleAnimationSegment *s, u32 m)
{
    if (s->animation.status & m)
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

#define TENTACLE_SPREAD_SET_OFFSET(out_, motion_, divisor_)                                        \
    do {                                                                                           \
        s32 count_ = (divisor_);                                                                   \
        (out_).x = Div((motion_).x, count_);                                                       \
        (out_).y = 0;                                                                              \
        (out_).z = Div((motion_).z, count_);                                                       \
    } while (0)

#endif
