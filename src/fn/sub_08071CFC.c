#include "scene_actor_registry.h"
void sub_0806FD5C(struct SceneActorRegistryManager *);
void sub_0806FDE0(struct SceneActorRegistryManager *);
void sub_0806FE88(struct SceneActorRegistryManager *);
s32 SceneActorRegistry_TickManager(struct SceneActorRegistryManager *manager)
{
    u32 mask;
    SceneActorRegistry_AdvanceInactiveActors(manager);
    sub_0806FD5C(manager);
    sub_0806FDE0(manager);
    sub_0806FE88(manager);
    SceneActorRegistry_UpdateBackgroundFlag(manager);
    sub_08070FA8(manager);
    mask = 0xFFFF3FFF;
    manager->flags &= mask;
    manager->frame++;
    return 0;
}
