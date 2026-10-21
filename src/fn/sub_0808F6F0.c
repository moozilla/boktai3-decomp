#include "scene_actor_chase.h"
#define Slime_RestoreSavedTarget sub_0808F6F0
void Slime_RestoreSavedTarget(struct SceneActorRegistryResetView *actor)
{
    if (Chase_TakeArrived(actor))
        Chase_SetCtl(actor, actor->savedTarget, 0);
    Callback_InvokeActor(actor, actor->update270);
    actor->counter2AC++;
}
