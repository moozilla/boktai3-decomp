#include "global.h"
#include "scene_actor_reset.h"

static inline u8 Peek(struct ChaseActor *s)
{
    return s->base.stepDone;
}
static inline u8 TakeEntry(struct ChaseActor *s)
{
    if (s->base.field2B1) {
        s->base.field2B1 = 0;
        return 1;
    }
    return 0;
}
void SceneActorControl_UpdateRouteResponse(struct ChaseActor *s)
{
    u8 entered = TakeEntry(s);
    if (entered) {
        void *fn = s->base.onEntry;
        u32 two = 2;
        u8 *ready = &s->base.field2B2;
        u32 zero = 0;
        *ready = 1;
        s->base.stepDone = zero;
        s->base.controlKind = two;
        s->base.target = fn;
        s->base.field2C8 = zero;
        {
            u32 *flags = &s->base.pendingControlFlags;
            s32 mask = -2;
            *flags &= mask;
        }
        s->base.pendingControl = zero;
    } else if (Peek(s)) {
        u8 *group = (u8 *)s + 0x3D4;
        u32 twelve = 12;
        void *fn;
        u32 kind;
        s->base.state2A8 = entered;
        s->base.phase2A9 = entered;
        s->base.behavior2AA = twelve;
        s->base.counter2AC = entered;
        s->base.field2B1 = 1;
        fn = *(void **)(group + 0x48);
        kind = 26;
        s->base.field2B2 = 1;
        s->base.stepDone = entered;
        s->base.controlKind = kind;
        s->base.target = fn;
        s->base.field2C8 = entered;
        {
            u32 *flags = &s->base.pendingControlFlags;
            s32 mask = -2;
            *flags &= mask;
        }
        s->base.pendingControl = entered;
    } else {
        ++s->base.counter2AC;
    }
}
