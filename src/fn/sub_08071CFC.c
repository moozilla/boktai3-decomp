#include "scene_actor_registry.h"
void SceneActorRegistry_DispatchMessages(struct SceneActorRegistryManager *);
void SceneActorRegistry_AdvanceCursor(struct SceneActorRegistryManager *);
void SceneActorRegistry_ScheduleAvailableActors(struct SceneActorRegistryManager *);
s32 SceneActorRegistry_TickManager(struct SceneActorRegistryManager *manager)
{
    u32 mask;
    SceneActorRegistry_AdvanceInactiveActors(manager);
    SceneActorRegistry_DispatchMessages(manager);
    SceneActorRegistry_AdvanceCursor(manager);
    SceneActorRegistry_ScheduleAvailableActors(manager);
    SceneActorRegistry_UpdateBackgroundFlag(manager);
    SceneActorRegistry_UpdateActors(manager);
    mask = 0xFFFF3FFF;
    manager->flags &= mask;
    manager->frame++;
    return 0;
}
