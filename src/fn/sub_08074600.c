// MATCHING_CONCESSION: register-pinning
#include "scene_actor_profile.h"
s32 SceneActorProfiles_FindOrAdd(u32 argument, u32 modeArgument)
{
    register u32 category asm("r4") = argument;
    u32 callCategory = argument;
    register u32 mode asm("r1") = modeArgument;
    s32 index;
    if (!mode) {
        asm("" : "+r"(category));
        index = SceneActorProfiles_FindDefaultIndex(category);
        if (index != -1)
            goto done;
        callCategory = category;
        mode = 0;
    }
    index = SceneActorProfiles_Add(callCategory, mode);
done:
    return index;
}
