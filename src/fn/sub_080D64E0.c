#include "global.h"
#include "scene_actor_reset.h"
void SceneActorControl_OnIdle(void);
struct MummyContext {
    u8 unknown0[0x6D8];
    u32 limit;
};
void Mummy_EnterIdleControl(struct ChaseActor *actor)
{
    struct MummyContext *context = actor->base.context;
    if (actor->base.tracking.distance < context->limit) {
        void *fn = SceneActorControl_OnIdle;
        u8 code = 0x10;
        u32 zero;
        u8 one;
        u8 zero8;
        u32 two;
        u8 *taken = &actor->base.field2B2;
        zero = 0;
        one = 1;
        *taken = one;
        actor->base.field2B0 = zero;
        actor->base.field2B5 = code;
        actor->base.target = fn;
        zero8 = 0;
        actor->base.field2C8 = zero;
        {
            u32 *flags = &actor->base.flags114;
            u32 mask = -2;
            *flags &= mask;
        }
        actor->base.word104 = zero;
        two = 2;
        actor->base.state2A8 = zero8;
        actor->base.phase2A9 = two;
        actor->base.behavior2AA = zero8;
        actor->base.counter2AC = zero;
        actor->base.field2B1 = one;
    } else {
        void *fn = SceneActorControl_OnIdle;
        u8 code = 0x10;
        u32 zero;
        u8 one;
        u8 zero8;
        u32 two;
        u8 *taken = &actor->base.field2B2;
        zero = 0;
        one = 1;
        *taken = one;
        actor->base.field2B0 = zero;
        actor->base.field2B5 = code;
        actor->base.target = fn;
        zero8 = 0;
        actor->base.field2C8 = zero;
        {
            u32 *flags = &actor->base.flags114;
            u32 mask = -2;
            *flags &= mask;
        }
        actor->base.word104 = zero;
        two = 2;
        actor->base.state2A8 = zero8;
        actor->base.phase2A9 = two;
        actor->base.behavior2AA = one;
        actor->base.counter2AC = zero;
        actor->base.field2B1 = one;
    }
}
