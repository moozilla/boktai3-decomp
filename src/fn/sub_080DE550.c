#include "global.h"
#include "scene_actor_profile.h"
#include "scene_actor_palette.h"


void SceneActor_RefreshLivingCountAndProfile(struct SceneActorRegistryActor *actor)
{
    u32 category = actor->category;
    s32 count = SceneActorRegistry_CountLivingByCategory(category);

    actor->livingCountByCategory = count;
    actor->profileIndex = SceneActorProfiles_FindOrAdd(category, count);
}
