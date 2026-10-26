#include "scene_actor_profile.h"
struct SceneActorProfile *SceneActorProfiles_Find(u32 id, u32 mode)
{
    struct SceneActorProfileList *list = (void *)gSceneActorRegistryManager->subsystem;
    struct SceneActorProfile *profile = list->head;
    s32 count = 0;
    if (profile) {
        do {
            if (profile->id == id && profile->mode == mode)
                return profile;
            profile = profile->next;
            count++;
            if (count > 11)
                break;
        } while (profile);
    }
    return 0;
}
