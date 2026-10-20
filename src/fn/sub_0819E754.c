#include "global.h"

#define Actor_SetHeadingVelocity sub_0819E754

struct HeadingVec {
    s16 x, y, z, pad;
};
struct HeadingActor {
    u8 pad[0x51];
    u8 heading;
    u8 pad52[0x5c - 0x52];
    struct HeadingVec velocity;
};
extern const s16 gUnk_086149C4[];
static inline s32 Scale(s32 x)
{
    return x >= 0 ? x >> 12 : -(-x >> 12);
}
void Actor_SetHeadingVelocity(struct HeadingActor *a, s32 speed)
{
    u8 *hp = &a->heading;
    struct HeadingVec *v = &a->velocity;
    u32 angle = *hp;
    v->x = Scale(gUnk_086149C4[(angle + 64) & 255] * speed);
    v->y = 0;
    v->z = Scale(gUnk_086149C4[angle & 255] * speed);
}
