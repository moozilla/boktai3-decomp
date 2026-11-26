#include "npc/manager/scene_actor_lifecycle.h"
#include "engine/ram.h"
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
