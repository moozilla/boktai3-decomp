#include "scene_actor_profile.h"
#define SceneActorProfiles_FreeAll sub_08073C04
void SceneActorProfiles_FreeAll(void)
{
    struct SceneActorProfileList *list = (void *)gUnk_020004A8->subsystem;
    struct SceneActorProfile *profile = list->head;
    while (profile) {
        struct SceneActorProfile *next = profile->next;
        Heap_Free(profile);
        profile = next;
    }
}
