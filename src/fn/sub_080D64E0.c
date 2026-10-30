#include "global.h"
#include "scene_actor_reset.h"
void SceneActorControl_OnIdle(void);
struct MummyContext {
    u8 unknown0[0x6D8];
    u32 limit;
};
void Mummy_EnterIdleControl(struct SceneActorRegistryResetView *actor)
{
    struct MummyContext *context = actor->context;
    if (actor->tracking.distance < context->limit) {
        void *fn = SceneActorControl_OnIdle;
        u8 code = 0x10;
        u32 zero;
        u8 one;
        u8 zero8;
        u32 two;
        u8 *taken = &actor->field2B2;
        zero = 0;
        one = 1;
        *taken = one;
        actor->field2B0 = zero;
        actor->field2B5 = code;
        actor->target = fn;
        zero8 = 0;
        actor->field2C8 = zero;
        {
            u32 *flags = &actor->flags114;
            u32 mask = -2;
            *flags &= mask;
        }
        actor->word104 = zero;
        two = 2;
        actor->state2A8 = zero8;
        actor->phase2A9 = two;
        actor->behavior2AA = zero8;
        actor->counter2AC = zero;
        actor->field2B1 = one;
    } else {
        void *fn = SceneActorControl_OnIdle;
        u8 code = 0x10;
        u32 zero;
        u8 one;
        u8 zero8;
        u32 two;
        u8 *taken = &actor->field2B2;
        zero = 0;
        one = 1;
        *taken = one;
        actor->field2B0 = zero;
        actor->field2B5 = code;
        actor->target = fn;
        zero8 = 0;
        actor->field2C8 = zero;
        {
            u32 *flags = &actor->flags114;
            u32 mask = -2;
            *flags &= mask;
        }
        actor->word104 = zero;
        two = 2;
        actor->state2A8 = zero8;
        actor->phase2A9 = two;
        actor->behavior2AA = one;
        actor->counter2AC = zero;
        actor->field2B1 = one;
    }
}
