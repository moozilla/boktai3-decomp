#define SceneActor_ResetQueuedState sub_080AB57C
#include "global.h"
void sub_0807FC90(void);
#include "scene_actor_reset.h"
void sub_080AB57C(struct SceneActorRegistryResetView *s)
{
    u32 two = 2;
    u32 zero;
    u8 one;
    void (*fn)(void);
    u8 code;
    u8 *field;
    u32 *flags;
    field = &s->state2A8;
    zero = 0;
    *field = zero;
    s->phase2A9 = two;
    s->behavior2AA = zero;
    s->counter2AC = zero;
    field = &s->field2B1;
    one = 1;
    *field = one;
    fn = sub_0807FC90;
    code = 0x10;
    s->field2B2 = one;
    s->field2B0 = zero;
    s->field2B5 = code;
    s->target = fn;
    s->field2C8 = zero;
    flags = &s->flags114;
    {
        u32 mask = -2;
        *flags &= mask;
    }
    s->word104 = zero;
}
