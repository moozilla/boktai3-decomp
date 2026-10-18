/* Tentative family and field names, based on static accesses. */
#define TimedSprite_InitDrawable SingletonSlot7_InitImpactSprite
#include "global.h"
struct Pair {
    u32 x, y;
};
struct TimedSprite {
    u8 unknown0[0x18];
    u8 sprite[0x2C];
    u8 resource[0x1C];
    u16 age;
    u8 unknown62[2];
    s32 duration;
};
void SingletonSlot7_InitImpactSprite(void *, void *, struct Pair *, s32);
u32 TimedSprite_Init(struct TimedSprite *p, struct Pair *pos, s32 frame, s32 duration)
{
    SingletonSlot7_InitImpactSprite(p->sprite, p->resource, pos, frame);
    {
        u16 *age = &p->age;
        u32 zero = 0;
        *age = zero;
        p->duration = duration;
        return zero;
    }
}
