#include "global.h"
#include "scene_actor_reset.h"
struct SceneActorBehaviorTimer {
    u8 unknown0[0x704];
    u16 timer;
};
void sub_080B9158(void);
void SceneActorControl_BeginTimedBehavior56(struct SceneActorRegistryResetView *s,
                                            struct SceneActorBehaviorTimer *other)
{
    switch (s->base.category) {
        case 6:
            other->timer = 60;
            break;
        case 5:
        case 7:
            other->timer = 120;
            break;
        default:
            return;
    }
    {
        void (*fn)(void) = sub_080B9158;
        u8 code = 0x38;
        u32 zero;
        u8 one;
        u8 zero8;
        u32 *flags;
        u8 *taken;
        u32 three;
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
        three = 3;
        s->base.state2A8 = three;
        s->base.phase2A9 = zero8;
        s->base.behavior2AA = zero8;
        s->base.counter2AC = zero;
        s->base.field2B1 = one;
        {
            u16 *flags = &s->base.flags2D6;
            u32 mask = 0xFFFFF636;
            *flags &= mask;
        }
    }
}
