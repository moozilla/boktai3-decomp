// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_control_phase.h"
void SceneActorControl_OnIdle(void);
#define FIELD(p, n, t) (*(t *)((u8 *)(p) + (n)))
// confidence=low: starts the actor callback at 0x234 then consumes its trigger.
#define SceneActorControl_UpdateCallbackTrigger sub_08130994
/* Control argument placement, state offsets and zero lifetimes still differ. */
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
static inline void SetState(struct SceneActorRegistryResetView *actor, u32 state, u32 zero,
                            u8 byteZero)
{
    actor->state2A8 = state;
    actor->phase2A9 = byteZero;
    actor->behavior2AA = byteZero;
    actor->counter2AC = zero;
    actor->field2B1 = 1;
}
static inline void SetFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    register u32 value asm("r0") = mask;
    asm("" : "+r"(value));
    value |= actor->flags2D6;
    asm("" : "+r"(value));
    actor->flags2D6 = value;
}
inline static void SetState_1(struct SceneActorRegistryResetView *actor, u32 state, u32 zero)
{
    actor->state2A8 = state;
    actor->phase2A9 = 0;
    actor->behavior2AA = 0;
    actor->counter2AC = zero;
    {
        typeof(actor->field2B1) *q1 = &actor->field2B1;
        *q1 = 1;
    }
}

void SceneActorControl_UpdateCallbackTrigger(struct SceneActorRegistryResetView *actor)
{
    u32 taken = SceneActorControl_TakeFlag(&actor->field2B1);
    if (taken) {
        BeginControlZero(actor, *((void **)(((u8 *)actor) + 0x234)), 0x11);
    } else if (actor->field2B0) {
        Trigger_ConsumePending(((u8 *)actor) + 0xC, 1);
        BeginControl(actor, SceneActorControl_OnIdle, 0x10, taken);
        SetState_1(actor, 3, taken);
        SetFlags(actor, 0x1000);
    } else {
        actor->counter2AC++;
    }
}
