#include "scene_actor_chase.h"
#define Dog_UpdateLeapControl sub_080E6B1C
void SceneActor_LeapTowardTarget(void);
void Dog_UpdateLeapControl(struct SceneActorRegistryResetView *a)
{
    if (Chase_TakeArrived(a))
        Chase_SetCtl(a, SceneActor_LeapTowardTarget, 0x23);
    if (a->field2B0)
        Chase_SetBehavior(a, 0x10);
    else {
        Callback_InvokeActor(a, a->update270);
        {
            u32 bit = 0x2000;
            u32 *flags = &a->flags2CC;
            *flags |= bit;
        }
    }
}
