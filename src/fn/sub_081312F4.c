// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_control_phase.h"
#define FIELD(p, n, t) (*(t *)((u8 *)(p) + (n)))
u8 sub_0812F3CC(struct SceneActorRegistryResetView *);
// confidence=low: selects a route return phase from motion and route flags.
#define SceneActorRoute_UpdateReturnEntry sub_081312F4

static inline void BeginControl(struct SceneActorRegistryResetView *actor, void *control, u32 kind,
                                u32 zero)
{
    u32 offset;
    {
        u8 *field = &actor->field2B2;
        *field = 1;
    }
    offset = 0x2B0;
    *((u8 *)actor + offset) = zero;
    actor->field2B5 = kind;
    offset -= 0x94;
    *(void **)((u8 *)actor + offset) = control;
    actor->field2C8 = zero;
    {
        u32 *flags = &actor->flags114;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = zero;
}
static inline void BeginControlZero(struct SceneActorRegistryResetView *actor, void *control,
                                    u32 kind)
{
    u32 offset;
    u32 zero;
    {
        u8 *field = &actor->field2B2;
        zero = 0;
        *field = 1;
    }
    offset = 0x2B0;
    *((u8 *)actor + offset) = zero;
    actor->field2B5 = kind;
    offset -= 0x94;
    *(void **)((u8 *)actor + offset) = control;
    actor->field2C8 = zero;
    {
        u32 *flags = &actor->flags114;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = zero;
}
static inline void State(struct SceneActorRegistryResetView *a, u32 behavior, u32 zero)
{
    a->state2A8 = zero;
    a->phase2A9 = zero;
    a->behavior2AA = behavior;
    a->counter2AC = zero;
    a->field2B1 = 1;
}
static inline void *SavedControl(u8 *group)
{
    void **field = (void *)(group + 0x214);
    return *field;
}
static inline u32 Finished(struct SceneActorRegistryResetView *a)
{
    register u32 offset asm("r3") = 0x2B0;
    asm("" : "+r"(offset));
    {
        register u8 *field asm("r0") = (void *)((u8 *)a + offset);
        return *field;
    }
}
static inline u32 MotionFlags(struct SceneActorRegistryResetView *a, u32 mask)
{
    register u32 offset asm("r2") = 0x2D4;
    asm("" : : "r"(offset));
    {
        register u16 *field asm("r0") = (void *)((u8 *)a + offset);
        return *field & mask;
    }
}
void SceneActorRoute_UpdateReturnEntry(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)&actor->group;
    u8 result;
    if (SceneActorControl_TakeFlag(&actor->field2B1))
        BeginControlZero(actor, actor->entryTarget224, 2);
    if ((u8)Callback_InvokeActor(actor, actor->contactQuery.opaque))
        return;
    result = sub_0812F3CC(actor);
    if (result)
        return;
    if (Finished(actor)) {
        u16 flags = MotionFlags(actor, 0x10);
        if (flags) {
            State(actor, 8, result);
            BeginControl(actor, SavedControl(group), 0x13, result);
        } else {
            u32 bits = actor->flags2CC & 0x10;
            if (bits)
                State(actor, 7, flags);
            else
                State(actor, 2, bits);
            BeginControlZero(actor, actor->entryTarget228, 1);
        }
    } else
        actor->counter2AC++;
}
