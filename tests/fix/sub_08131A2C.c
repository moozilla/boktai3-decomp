#include "global.h"
#include "scene_actor_control_phase.h"

u8 sub_0812F3CC(void *);

static inline void SetCtl(struct SceneActorRegistryResetView *actor, void *cb, u32 k)
{
    void *callback = cb;
    u32 kind = k;
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = callback;
    actor->field2C8 = 0;
    {
        u32 *field = &actor->flags114;
        u32 mask = ~1;
        *field &= mask;
    }
    actor->word104 = 0;
}

static inline void SetState(struct SceneActorRegistryResetView *actor, u32 s, u32 p, u32 b)
{
    actor->state2A8 = s;
    actor->phase2A9 = p;
    actor->behavior2AA = b;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

void sub_08131A2C(struct SceneActorRegistryResetView *actor)
{
    u8 *p = (u8 *)actor;
    if (SceneActorControl_TakeFlag(&actor->field2B1))
        SetCtl(actor, *(void **)(p + 0x22C), 0);
    if (!(u8)Callback_InvokeActor(actor, *(void **)(p + 0x270)) && !sub_0812F3CC(actor)) {
        if (actor->counter2AC > 59)
            SetState(actor, 0, 0, 2);
        else
            actor->counter2AC++;
    }
}
