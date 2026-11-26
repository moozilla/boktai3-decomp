#include "actors/scene_actor_resources_and_lifecycle/scene_actor_lifecycle.h"
#include "engine/shared/ram.h"
int SceneActorManager_Init(struct SceneActorManager *);
void SceneActorManager_Update(void);
void SceneActorManager_Destroy(void);
/* Registry routine 286F: returns the scene actor manager (gSceneActorListManager,
   which also holds the message packet queue), creating its task on first use. */
struct SceneActorManager *SceneActorManager_Create(void)
{
    struct SceneActorManager *p;
    if (gSceneActorListManager)
        return gSceneActorListManager;
    p = Task_Create(9, 0x43c);
    if (p) {
        Task_SetCallbacks(p, SceneActorManager_Update, SceneActorManager_Destroy);
        if (SceneActorManager_Init(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
