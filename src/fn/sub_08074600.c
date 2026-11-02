#include "scene_actor_profile.h"
s32 SceneActorProfiles_FindOrAdd(u32 category, u32 mode)
{
    s32 index;
    if (mode) {
        index = SceneActorProfiles_Add(category, mode);
    } else {
        index = SceneActorProfiles_FindDefaultIndex(category);
        if (index == -1)
            index = SceneActorProfiles_Add(category, 0);
    }
    return index;
}
