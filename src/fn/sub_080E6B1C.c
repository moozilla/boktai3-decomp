#include "scene_actor_chase.h"
void SceneActor_LeapTowardTarget(void);
void Dog_UpdateLeapControl(struct ChaseActor *a)
{
    if (Chase_TakeArrived(a))
        Chase_SetCtl(a, SceneActor_LeapTowardTarget, 0x23);
    if (a->base.field2B0)
        Chase_SetBehavior(a, 0x10);
    else {
        Callback_InvokeActor(a, a->base.contactQuery.opaque);
        {
            u32 bit = 0x2000;
            u32 *flags = &a->base.flags2CC;
            *flags |= bit;
        }
    }
}
