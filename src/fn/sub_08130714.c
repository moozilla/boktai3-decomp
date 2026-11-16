#include "global.h"
#include "scene_actor_control_phase.h"

// confidence=low: approaches the active route descriptor and consumes its trigger on arrival.

s32 Callback_InvokeActor(void *, void *);
void Anim_SetDurationScale(void *, u32);
void Anim_SetFrameDurationScale(void *, u32);
u8 SceneActorBehavior_AccelerateTowardDescriptor(void *, void *, u32, u32, s32);

static inline void SetControl(struct SceneActorRegistryResetView *actor, void *control, u32 kind)
{
    actor->base.field2B2 = 1;
    actor->base.field2B0 = 0;
    actor->base.field2B5 = kind;
    actor->base.target = control;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->base.word104 = 0;
}
static inline void ResetState(struct SceneActorRegistryResetView *actor)
{
    u32 state = 2;
    actor->base.state2A8 = state;
    actor->base.phase2A9 = 0;
    actor->base.behavior2AA = state;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}
void SceneActorControl_UpdateTriggerApproach(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)&actor->group.common;
    s32 speed;
    u16 *scale;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        SetControl(actor, actor->base.entryTarget228, 1);
    } else {
        SceneActorAnimation_SelectDirectionalEntry(actor, 1);
        speed = Callback_InvokeActor(actor, *(void **)((u8 *)actor + 0x260));
        {
            u32 mask = 0xa02f;
            if (actor->base.flags2D4 & mask) {
                u16 *p = &actor->base.field2C2;
                *p = 64;
                scale = p;
            } else {
                u16 *p = &actor->base.field2C2;
                u32 value = *p;
                scale = p;
                if (!value)
                    *p = 64;
            }
        }
        if (!actor->base.drawableKind)
            Anim_SetDurationScale((u8 *)actor->base.drawable + 0x48, *scale);
        else
            Anim_SetFrameDurationScale((u8 *)actor->base.drawable + 0x20, *scale);
        if (SceneActorBehavior_AccelerateTowardDescriptor(actor, *(void **)(group + 0x1fc), 32, 32,
                                                          speed)) {
            Trigger_ConsumePending((u8 *)actor + 0xc, 1);
            ResetState(actor);
            SetControl(actor, actor->base.savedTarget, 0);
        } else {
            actor->base.counter2AC++;
        }
    }
}
