#include "scene/actor/scene_actor_registry.h"
void SceneActorRegistry_SetObjectContext(struct ChaseActor *actor, void *context, u32 kind,
                                         void *object)
{
    actor->base.context = context;
    actor->base.object = object;
    actor->base.drawableKind = kind;
}
