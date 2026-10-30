#include "global.h"
#include "scene_actor_control_phase.h"

// confidence=low: approaches the active route descriptor and consumes its trigger on arrival.

s32 Callback_InvokeActor(void *, void *);
void Anim_SetDurationScale(void *, u32);
void Anim_SetFrameDurationScale(void *, u32);
u8 SceneActorBehavior_AccelerateTowardDescriptor(void *, void *, u32, u32, s32);

static inline void SetControl(struct SceneActorRegistryResetView *actor, void *control, u32 kind)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = control;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
}
static inline void ResetState(struct SceneActorRegistryResetView *actor)
{
    u32 state = 2;
    actor->state2A8 = state;
    actor->phase2A9 = 0;
    actor->behavior2AA = state;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
void SceneActorControl_UpdateTriggerApproach(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)&actor->group.common;
    s32 speed;
    u16 *scale;
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        SetControl(actor, actor->entryTarget228, 1);
    } else {
        SceneActorAnimation_SelectDirectionalEntry(actor, 1);
        speed = Callback_InvokeActor(actor, *(void **)((u8 *)actor + 0x260));
        {
            u32 mask = 0xa02f;
            if (actor->flags2D4 & mask) {
                u16 *p = &actor->field2C2;
                *p = 64;
                scale = p;
            } else {
                u16 *p = &actor->field2C2;
                u32 value = *p;
                scale = p;
                if (!value)
                    *p = 64;
            }
        }
        if (!actor->drawableKind)
            Anim_SetDurationScale((u8 *)actor->drawable + 0x48, *scale);
        else
            Anim_SetFrameDurationScale((u8 *)actor->drawable + 0x20, *scale);
        if (SceneActorBehavior_AccelerateTowardDescriptor(actor, *(void **)(group + 0x1fc), 32, 32,
                                                          speed)) {
            Trigger_ConsumePending((u8 *)actor + 0xc, 1);
            ResetState(actor);
            SetControl(actor, actor->savedTarget, 0);
        } else {
            actor->counter2AC++;
        }
    }
}
