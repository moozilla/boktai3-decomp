#include "global.h"
#include "scene_actor_reset.h"
void SceneActorControl_OnIdle(void);
// confidence=low: distance band uses context bounds at 0x708 and 0x70C.
#define Skeleton_TryEnterIdleRange sub_080B6480
u32 Skeleton_TryEnterIdleRange(struct ChaseActor *s)
{
    u8 *context = s->base.context;
    if (s->base.tracking.distance > *(u32 *)(context + 0x708) &&
        s->base.tracking.distance < *(u32 *)(context + 0x70C)) {
        void (*fn)(void) = SceneActorControl_OnIdle;
        u8 code = 0x10;
        u32 zero;
        u8 one;
        u8 zero8;
        u32 *flags;
        u8 *taken;
        u32 two;
        taken = &s->base.field2B2;
        zero = 0;
        one = 1;
        *taken = one;
        s->base.field2B0 = zero;
        s->base.field2B5 = code;
        s->base.target = fn;
        zero8 = 0;
        s->base.field2C8 = zero;
        flags = &s->base.flags114;
        {
            u32 mask = -2;
            *flags &= mask;
        }
        s->base.word104 = zero;
        two = 2;
        s->base.state2A8 = zero8;
        s->base.phase2A9 = two;
        s->base.behavior2AA = zero8;
        s->base.counter2AC = zero;
        s->base.field2B1 = one;
        return 1;
    }
    return 0;
}
