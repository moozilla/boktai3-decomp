#include "scene/actor/scene_actor_registry.h"
void SceneActorRegistry_DestroyActors(void)
{
    struct SceneActorRegistryNode *node = gSceneActorRegistryNode;
    struct ChaseActor *actor;
    if (node) {
        while ((actor = node->actor) != 0) {
            void *context;
            s32 (*destroy)(void *);
            node = node->next;
            context = actor->base.context;
            destroy = actor->base.destroy;
            Callback_InvokeActor(context, destroy);
            Heap_Free(context);
        }
    }
}
