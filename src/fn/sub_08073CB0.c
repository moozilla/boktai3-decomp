#include "scene_actor_profile.h"
#define SceneActorProfiles_GetEntryAddress sub_08073CB0
/* Integer address arithmetic preserves the lookup miss result without forming a null-derived pointer. */
u32 SceneActorProfiles_GetEntryAddress(u32 id, u32 mode, s32 index)
{
    u32 profile = (u32)sub_08073C70(id, mode);
    u32 offset = (u32)index << 2;
    offset += 8;
    return profile + offset;
}
