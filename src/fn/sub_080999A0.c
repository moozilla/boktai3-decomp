#include "scene_actor_chase.h"

void sub_080999A0(struct SceneActorRegistryResetView *actor)
{
    if (Chase_TakeArrived(actor)) {
        Chase_SetCtl(actor, actor->savedTarget, 0);
    } else {
        {
            u32 mask = 0x800;
            actor->flags2C0 |= mask;
        }
        if (actor->counter2AC == 0)
            Trigger_ConsumePending((u8 *)actor + 0xc, 1);
        actor->counter2AC++;
    }
}
