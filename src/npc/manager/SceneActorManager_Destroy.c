#include "npc/manager/scene_actor_lifecycle.h"
#include "engine/ram.h"
int SceneActorManager_Destroy(struct SceneActorManager *manager)
{
    struct SceneActor *actor = manager->head;
    while (actor) {
        struct SceneActor *previous = actor->previous;
        if (actor->destroy)
            actor->destroy(manager, actor);
        Actor_Destroy(actor);
        actor = previous;
    }
    gSceneActorListManager = 0;
    return 0;
}
