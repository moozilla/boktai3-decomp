#include "engine/shared/scene_actor_terrain_reaction.h"
#include "engine/shared/global.h"
#include "engine/shared/scene_actor_control_phase.h"

// confidence=low: approaches the active route descriptor and consumes its trigger on arrival.

s32 Callback_InvokeActor(void *, void *);
void Anim_SetDurationScale(void *, u32);
void Anim_SetFrameDurationScale(void *, u32);
u8 SceneActorBehavior_AccelerateTowardDescriptor(void *, void *, u32, u32, s32);

void SceneActorControl_UpdateTriggerApproach(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    s32 speed;
    u16 *scale;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        SceneActorTerrain_InstallControl(actor, actor->base.onAlternateEntry, 1);
    } else {
        SceneActorAnimation_SelectDirectionalEntry(actor, 1);
        speed = Callback_InvokeActor(actor, actor->base.animationQuery.opaque);
        {
            if (SceneActorBehavior_MotionBits(0xA02F, actor)) {
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
        if (SceneActorBehavior_AccelerateTowardDescriptor(actor, group->route, 32, 32,
                                                          speed)) {
            Trigger_ConsumePending(actor->base.receiver, 1);
            SceneActorTerrain_EnterState(actor, 2, 0, 2);
            SceneActorTerrain_InstallControl(actor, actor->base.onStanding, 0);
        } else {
            actor->base.counter2AC++;
        }
    }
}
