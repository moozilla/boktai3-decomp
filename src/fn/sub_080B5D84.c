#define SceneActorControl_BeginTimedBehavior56 sub_080B5D84
#include "global.h"
#include "scene_actor_reset.h"
struct SceneActorBehaviorTimer {
    u8 unknown0[0x704];
    u16 timer;
};
void sub_080B9158(void);
void sub_080B5D84(struct SceneActorRegistryResetView *s, struct SceneActorBehaviorTimer *other)
{
    switch (s->category) {
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
        three = 3;
        s->state2A8 = three;
        s->phase2A9 = zero8;
        s->behavior2AA = zero8;
        s->counter2AC = zero;
        s->field2B1 = one;
        {
            u16 *flags = &s->flags2D6;
            u32 mask = 0xFFFFF636;
            *flags &= mask;
        }
    }
}
