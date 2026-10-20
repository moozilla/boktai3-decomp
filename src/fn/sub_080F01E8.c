#include "global.h"
#include "scene_actor_chase.h"

void sub_080EE148(struct SceneActorRegistryResetView *);
void SceneActorControl_OnIdle(void);

void Ivy_UpdateIdle(struct SceneActorRegistryResetView *actor)
{
    if (Chase_TakeArrived(actor))
        Chase_SetCtl(actor, sub_080EE148, 0x2E);
    if (actor->field2B0) {
        Chase_SetCtl(actor, SceneActorControl_OnIdle, 0x10);
        Chase_SetState(actor, 3, 0, 0);
        {
            u32 bit = 0x1000;
            actor->flags2D6 = bit | actor->flags2D6;
        }
    }
}
