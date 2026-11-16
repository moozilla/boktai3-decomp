#include "global.h"
#include "scene_actor_chase.h"

extern const u8 gUnk_0824E218[];

void SceneActorReaction_PlayAnimationBUntilComplete(void);
void SceneActorBehavior_CallActionScript(void *);
void Vorn_SelectRecoveryAction(void *, const u8 *, u32);

void Vorn_UpdateScriptedAction(struct ChaseActor *actor)
{
    if (Chase_TakeArrived(actor)) {
        Chase_SetCtl(actor, SceneActorReaction_PlayAnimationBUntilComplete, 10);
        actor->base.body.direction = actor->base.tracking.heading;
        SceneActorBehavior_CallActionScript(actor);
    }
    if (actor->base.stepDone) {
        u8 *group = (u8 *)actor;
        {
            u16 *h = (u16 *)(group + 0x414);
            u32 flag = 4;
            flag |= *h;
            *h = flag;
        }
        {
            u32 flag = 0x80;
            u16 *flags = &actor->base.motionFlags;
            flag |= *flags;
            *flags = flag;
        }
        {
            u16 value = *(u16 *)(group + 0x416);
            *(u16 *)(group + 0x418) = value;
        }
        Vorn_SelectRecoveryAction(actor, gUnk_0824E218, 0x27);
    } else {
        actor->base.counter2AC++;
    }
}
