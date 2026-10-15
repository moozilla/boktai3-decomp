// MATCHING_CONCESSION: register-pinning
#include "scene_actor_registry.h"
#define SceneActorRegistry_SetObjectContext sub_08076020
void SceneActorRegistry_SetObjectContext(struct SceneActorRegistryActor *actor, void *context,
                                         u32 kind, void *object)
{
    register u32 offset asm("r5") = 0x3d0;
    register void **field asm("r4");
    field = (void *)((u8 *)actor + offset);
    *field = context;
    actor->object = object;
    actor->drawableKind = kind;
}
