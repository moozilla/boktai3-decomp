#include "global.h"
#include "scene_actor_chase.h"
#include "debug.h"
// confidence=low: waits for a timer, then replaces response slot 0 and restarts the behavior.
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
void Mummy_WaitReplaceResponse(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorRegistryControlGroup *group = &actor->group.common;
    if (Chase_TakeArrived(actor)) {
        DEBUG_PRINT();
    }
    if (!(u8)Callback_InvokeActor(actor, actor->contactQuery.opaque)) {
        if ((s32)actor->counter2AC >= group->gate.counter) {
            SceneActorAttachment_ReplaceResponseSlot(actor, 0, 2, actor->category);
            Chase_SetBehavior(actor, 2);
        } else {
            actor->counter2AC++;
        }
    }
}
