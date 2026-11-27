#include "global.h"
#include "scene/actor/scene_actor_reset.h"
void SceneActorReaction_BeginContactResponse(struct ChaseActor *);
static inline u8 SceneActor_IsDetached(struct ChaseActor *actor)
{
    if (!actor->base.object)
        return 1;
    return 0;
}
void SceneActor_RunControlAndFallback(struct ChaseActor *actor)
{
    if (!(u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque)) {
        if (!SceneActor_IsDetached(actor))
            SceneActorReaction_BeginContactResponse(actor);
    }
}
