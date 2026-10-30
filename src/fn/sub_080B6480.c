#include "global.h"
#include "scene_actor_reset.h"
void SceneActorControl_OnIdle(void);
// confidence=low: distance band uses context bounds at 0x708 and 0x70C.
#define Skeleton_TryEnterIdleRange sub_080B6480
u32 Skeleton_TryEnterIdleRange(struct SceneActorRegistryResetView *s)
{
    u8 *context = s->context;
    if (s->tracking.distance > *(u32 *)(context + 0x708) && s->tracking.distance < *(u32 *)(context + 0x70C)) {
        void (*fn)(void) = SceneActorControl_OnIdle;
        u8 code = 0x10;
        u32 zero;
        u8 one;
        u8 zero8;
        u32 *flags;
        u8 *taken;
        u32 two;
        taken = &s->field2B2;
        zero = 0;
        one = 1;
        *taken = one;
        s->field2B0 = zero;
        s->field2B5 = code;
        s->target = fn;
        zero8 = 0;
        s->field2C8 = zero;
        flags = &s->flags114;
        {
            u32 mask = -2;
            *flags &= mask;
        }
        s->word104 = zero;
        two = 2;
        s->state2A8 = zero8;
        s->phase2A9 = two;
        s->behavior2AA = zero8;
        s->counter2AC = zero;
        s->field2B1 = one;
        return 1;
    }
    return 0;
}
