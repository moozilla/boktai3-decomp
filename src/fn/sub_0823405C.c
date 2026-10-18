#include "global.h"
struct MovingEffectOwner {
    u8 pad0[0x4ef];
    u8 activeCount;
};
struct EffectPosition {
    u16 xFraction;
    s16 x;
    u32 y;
};
struct MovingEffect {
    u8 pad0[0x18];
    struct MovingEffectOwner *owner;
    u8 pad1c[0x38 - 0x1c];
    struct EffectPosition position;
    u8 pad40[0x64 - 0x40];
    struct EffectPosition sourcePosition;
    u8 pad6c[0x1da - 0x6c];
    u16 age;
};
void Task_Kill(void *);
void MovingEffect_AdvanceLifetime(struct MovingEffect *s)
{
    s->sourcePosition.x -= 20;
    s->position = s->sourcePosition;
    if (++s->age > 2) {
        s->owner->activeCount--;
        Task_Kill(s);
    }
}
