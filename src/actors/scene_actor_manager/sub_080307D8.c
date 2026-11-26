#include "actors/scene_actor_resources_and_lifecycle/scene_actor_lifecycle.h"
#include "engine/shared/ram.h"
int SceneActorManager_Destroy(struct SceneActorManager *m)
{
    struct SceneActor *p = m->head;
    while (p) {
        struct SceneActor *prev = p->previous;
        if (p->destroy)
            p->destroy(m, p);
        Actor_Destroy(p);
        p = prev;
    }
    return (gSceneActorListManager = 0);
}
