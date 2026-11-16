#include "global.h"
#include "scene_actor_profile.h"
#include "scene_actor_palette.h"

void SceneActor_RefreshLivingCountAndProfile(struct ChaseActor *actor)
{
    u32 category = actor->base.category;
    s32 count = SceneActorRegistry_CountLivingByCategory(category);

    actor->base.profileMode = count;
    actor->base.profileId = SceneActorProfiles_FindOrAdd(category, count);
}
