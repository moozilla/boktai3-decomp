#include "scene_actor_profile.h"
void SceneActorProfiles_FreeAll(void)
{
    struct SceneActorProfileList *list = (void *)gSceneActorRegistryManager->subsystem;
    struct SceneActorProfile *profile = list->head;
    while (profile) {
        struct SceneActorProfile *next = profile->next;
        Heap_Free(profile);
        profile = next;
    }
}
