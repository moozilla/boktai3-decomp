#include "global.h"
#include "scene_actor_registry.h"
#include "scene_actor_palette.h"
#include "scene_actor_profile.h"

void SceneActor_UpdateProfileForCategory(struct SceneActorRegistryActor *actor)
{
    s32 category = actor->category;
    s32 count = SceneActorRegistry_CountLivingByCategory(category);

    actor->livingCountByCategory = count;

    switch (category) {
        case 5:
            actor->profileIndex = SceneActorProfiles_FindOrAdd(5, count);
            break;
        case 6:
            actor->profileIndex = SceneActorProfiles_FindOrAdd(6, count);
            break;
        case 7:
            actor->profileIndex = SceneActorProfiles_FindOrAdd(7, count);
            break;
        case 8:
            actor->profileIndex = SceneActorProfiles_FindOrAdd(8, count);
            break;
    }
}
