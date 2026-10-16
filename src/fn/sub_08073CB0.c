#include "scene_actor_profile.h"
/* Integer address arithmetic preserves the lookup miss result without forming a null-derived pointer. */
u32 SceneActorProfiles_GetEntryAddress(u32 id, u32 mode, s32 index)
{
    u32 profile = (u32)SceneActorProfiles_Find(id, mode);
    u32 offset = (u32)index << 2;
    offset += 8;
    return profile + offset;
}
