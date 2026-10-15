#include "scene_actor_registry.h"
void SceneActorRegistry_DestroyActors(void)
{
    struct SceneActorRegistryNode *node = gUnk_020004AC;
    struct SceneActorRegistryActor *actor;
    if (node) {
        while ((actor = node->actor) != 0) {
            void *context;
            s32 (*destroy)(void *);
            node = node->next;
            context = actor->context;
            destroy = actor->destroy;
            sub_0824923C(context, destroy);
            Heap_Free(context);
        }
    }
}
