#include "scene_actor_chase.h"
void sub_0808E780(void);
void Slime_RunUpdateThenWait(struct SceneActorRegistryResetView *actor)
{
    u8 result;
    if (actor->field2B1)
        actor->field2B1 = 0;
    result = Callback_InvokeActor(actor, actor->contactQuery.opaque);
    if (!result) {
        Chase_SetCtl(actor, sub_0808E780, 0x27);
        Chase_SetState(actor, 3, 1, 0);
    }
    actor->counter2AC++;
}
