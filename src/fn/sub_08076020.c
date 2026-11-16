#include "scene_actor_registry.h"
void SceneActorRegistry_SetObjectContext(struct ChaseActor *actor, void *context, u32 kind,
                                         void *object)
{
    u32 offset = 0x3d0;
    void **field;
    field = (void *)((u8 *)actor + offset);
    *field = context;
    actor->base.object = object;
    actor->base.drawableKind = kind;
}
