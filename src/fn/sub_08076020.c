#include "scene_actor_registry.h"
void SceneActorRegistry_SetObjectContext(struct ChaseActor *actor, void *context, u32 kind,
                                         void *object)
{
    void **field;
    field = (void *)((u8 *)actor + 0x3d0);
    *field = context;
    actor->base.object = object;
    actor->base.drawableKind = kind;
}
