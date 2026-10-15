#ifndef GUARD_SCENE_ACTOR_PROFILE_H
#define GUARD_SCENE_ACTOR_PROFILE_H
#include "scene_actor_registry.h"
/* Tentative profile records; entry meanings await their consumers. */
struct SceneActorProfileEntry {
    u8 value, flags, state, unknown3;
};
struct SceneActorProfile {
    struct SceneActorProfile *next;
    u8 id, mode, category, entryCount;
    struct SceneActorProfileEntry entries[30];
};
struct SceneActorProfileList {
    struct SceneActorProfile *head;
    s32 count;
};
s32 SceneActorProfiles_FindDefaultIndex(u32);
struct SceneActorProfile *SceneActorProfiles_Find(u32, u32);
u32 SceneActorProfiles_GetEntryAddress(u32, u32, s32);
s32 SceneActorProfiles_Add(u32, u32);
s32 SceneActorProfiles_FindOrAdd(u32, u32);
#endif
