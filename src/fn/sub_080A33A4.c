#include "global.h"
#include "scene_actor_reset.h"

static inline u8 Peek(struct SceneActorRegistryResetView *s)
{
    return s->field2B0;
}
static inline u8 TakeEntry(struct SceneActorRegistryResetView *s)
{
    if (s->field2B1) {
        s->field2B1 = 0;
        return 1;
    }
    return 0;
}
void SceneActorControl_UpdateRouteResponse(struct SceneActorRegistryResetView *s)
{
    u8 entered = TakeEntry(s);
    if (entered) {
        void *fn = s->entryTarget224;
        u32 two = 2;
        u8 *ready = &s->field2B2;
        u32 zero = 0;
        *ready = 1;
        s->field2B0 = zero;
        s->field2B5 = two;
        s->target = fn;
        s->field2C8 = zero;
        {
            u32 *flags = &s->flags114;
            s32 mask = -2;
            *flags &= mask;
        }
        s->word104 = zero;
    } else if (Peek(s)) {
        u8 *group = (u8 *)s + 0x3D4;
        u32 twelve = 12;
        void *fn;
        u32 kind;
        s->state2A8 = entered;
        s->phase2A9 = entered;
        s->behavior2AA = twelve;
        s->counter2AC = entered;
        s->field2B1 = 1;
        fn = *(void **)(group + 0x48);
        kind = 26;
        s->field2B2 = 1;
        s->field2B0 = entered;
        s->field2B5 = kind;
        s->target = fn;
        s->field2C8 = entered;
        {
            u32 *flags = &s->flags114;
            s32 mask = -2;
            *flags &= mask;
        }
        s->word104 = entered;
    } else {
        ++s->counter2AC;
    }
}
