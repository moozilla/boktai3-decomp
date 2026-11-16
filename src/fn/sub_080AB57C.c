#include "global.h"
void SceneActorControl_OnIdle(void);
#include "scene_actor_reset.h"
void SceneActor_ResetQueuedState(struct SceneActorRegistryResetView *s)
{
    u32 two = 2;
    u32 zero;
    u8 one;
    void (*fn)(void);
    u8 code;
    u8 *field;
    u32 *flags;
    field = &s->base.state2A8;
    zero = 0;
    *field = zero;
    s->base.phase2A9 = two;
    s->base.behavior2AA = zero;
    s->base.counter2AC = zero;
    field = &s->base.field2B1;
    one = 1;
    *field = one;
    fn = SceneActorControl_OnIdle;
    code = 0x10;
    s->base.field2B2 = one;
    s->base.field2B0 = zero;
    s->base.field2B5 = code;
    s->base.target = fn;
    s->base.field2C8 = zero;
    flags = &s->base.flags114;
    {
        u32 mask = -2;
        *flags &= mask;
    }
    s->base.word104 = zero;
}
