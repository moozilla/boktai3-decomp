#include "scene_actor_chase.h"
void Slime_RestoreSavedTarget(struct SceneActorRegistryResetView *actor)
{
    if (Chase_TakeArrived(actor))
        Chase_SetCtl(actor, actor->savedTarget, 0);
    Callback_InvokeActor(actor, actor->update270);
    actor->counter2AC++;
}
