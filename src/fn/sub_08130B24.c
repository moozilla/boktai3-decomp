#include "global.h"
#include "scene_actor_reset.h"
void sub_0812F3CC(struct SceneActorRegistryResetView *);
static inline u8 SceneActor_IsDetached(struct SceneActorRegistryResetView *actor)
{
    if (!actor->object)
        return 1;
    return 0;
}
void SceneActor_RunControlAndFallback(struct SceneActorRegistryResetView *actor)
{
    if (!(u8)Callback_InvokeActor(actor, actor->contactQuery.opaque)) {
        if (!SceneActor_IsDetached(actor))
            sub_0812F3CC(actor);
    }
}
