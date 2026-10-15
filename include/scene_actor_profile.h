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
s32 sub_08073C2C(u32);
struct SceneActorProfile *sub_08073C70(u32, u32);
u32 sub_08073CB0(u32, u32, s32);
s32 sub_08073CC4(u32, u32);
s32 sub_08074600(u32, u32);
#endif
