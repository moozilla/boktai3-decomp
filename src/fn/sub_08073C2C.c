#include "scene_actor_profile.h"
#define SceneActorProfiles_FindDefaultIndex sub_08073C2C
s32 SceneActorProfiles_FindDefaultIndex(u32 category)
{
    struct SceneActorProfileList *list = (void *)gUnk_020004A8->subsystem;
    struct SceneActorProfile *head = list->head;
    struct SceneActorProfile *profile;
    s32 count;
    if (head)
        goto initialize;
    goto missing;
found:
    return profile->id;
initialize:
    profile = head;
    count = 0;
scan:
    if (profile->category == category && profile->mode == 0)
        goto found;
    profile = profile->next;
    count++;
    if (count > 11)
        goto missing;
    if (profile)
        goto scan;
missing:
    return -1;
}
