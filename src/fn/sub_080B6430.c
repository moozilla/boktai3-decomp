#include "global.h"
#include "scene_actor_registry.h"
#include "scene_actor_palette.h"
#include "scene_actor_profile.h"

void SceneActor_UpdateProfileForCategory(struct ChaseActor *actor)
{
    s32 category = actor->base.category;
    s32 count = SceneActorRegistry_CountLivingByCategory(category);

    actor->base.profileMode = count;

    switch (category) {
        case 5:
            actor->base.profileId = SceneActorProfiles_FindOrAdd(5, count);
            break;
        case 6:
            actor->base.profileId = SceneActorProfiles_FindOrAdd(6, count);
            break;
        case 7:
            actor->base.profileId = SceneActorProfiles_FindOrAdd(7, count);
            break;
        case 8:
            actor->base.profileId = SceneActorProfiles_FindOrAdd(8, count);
            break;
    }
}
