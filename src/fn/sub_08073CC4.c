#include "scene_actor_profile.h"
/* The cursor stays within the 128-byte record; bytes8..10 are each entry's payload. */
static inline void SceneActorProfile_SetEntry(struct SceneActorProfile *profile, u32 index,
                                              u32 value, u32 flags, u32 state)
{
    u8 *cursor = (u8 *)profile + index * 4;
    cursor[8] = value;
    cursor[9] = flags;
    cursor[10] = state;
}
s32 SceneActorProfiles_Add(u32 category, u32 mode)
{
    struct SceneActorProfileList *registry = (void *)gSceneActorRegistryManager->subsystem;
    struct SceneActorProfile *profile;
    if (registry->count > 11)
        return -1;
    profile = Heap_Alloc(0x80);
    if (!profile)
        return -1;
    Memory_Clear(profile, 0x80);
    profile->next = registry->head;
    registry->head = profile;
    registry->count++;
    profile->id = registry->count;
    profile->mode = mode;
    profile->category = category;
    profile->entryCount = 30;
    switch (category) {
        case 0:
            return -1;
        case 1:
            SceneActorProfile_SetEntry(profile, 0, 0, 0, 0);
            SceneActorProfile_SetEntry(profile, 1, 1, 0, 0);
            SceneActorProfile_SetEntry(profile, 2, 2, 0, 0);
            SceneActorProfile_SetEntry(profile, 3, 3, 0, 0);
            SceneActorProfile_SetEntry(profile, 5, 6, 0, 0);
            SceneActorProfile_SetEntry(profile, 6, 5, 0, 0);
            SceneActorProfile_SetEntry(profile, 7, 7, 0, 0);
            SceneActorProfile_SetEntry(profile, 8, 8, 0, 0);
            SceneActorProfile_SetEntry(profile, 9, 7, 0, 0);
            SceneActorProfile_SetEntry(profile, 10, 4, 0, 0);
            break;
        case 2:
            SceneActorProfile_SetEntry(profile, 0, 0, 0, 0);
            SceneActorProfile_SetEntry(profile, 1, 3, 0, 0);
            SceneActorProfile_SetEntry(profile, 2, 5, 0, 0);
            SceneActorProfile_SetEntry(profile, 3, 4, 0, 0);
            SceneActorProfile_SetEntry(profile, 5, 2, 0, 0);
            SceneActorProfile_SetEntry(profile, 14, 1, 0, 0);
            break;
        case 3:
            SceneActorProfile_SetEntry(profile, 0, 0, 0, 0);
            SceneActorProfile_SetEntry(profile, 11, 0, 16, 0);
            SceneActorProfile_SetEntry(profile, 1, 1, 0, 0);
            SceneActorProfile_SetEntry(profile, 2, 2, 0, 0);
            SceneActorProfile_SetEntry(profile, 3, 3, 0, 0);
            SceneActorProfile_SetEntry(profile, 6, 4, 0, 0);
            SceneActorProfile_SetEntry(profile, 7, 5, 0, 0);
            SceneActorProfile_SetEntry(profile, 10, 6, 0, 0);
            SceneActorProfile_SetEntry(profile, 14, 7, 0, 0);
            SceneActorProfile_SetEntry(profile, 15, 8, 0, 0);
            break;
        case 4:
            SceneActorProfile_SetEntry(profile, 0, 7, 0, 0);
            SceneActorProfile_SetEntry(profile, 1, 8, 0, 0);
            SceneActorProfile_SetEntry(profile, 2, 9, 0, 0);
            SceneActorProfile_SetEntry(profile, 3, 2, 0, 0);
            SceneActorProfile_SetEntry(profile, 5, 10, 0, 0);
            SceneActorProfile_SetEntry(profile, 6, 5, 0, 0);
            SceneActorProfile_SetEntry(profile, 7, 6, 0, 0);
            SceneActorProfile_SetEntry(profile, 8, 5, 0, 0);
            SceneActorProfile_SetEntry(profile, 9, 6, 0, 0);
            SceneActorProfile_SetEntry(profile, 10, 4, 0, 0);
            SceneActorProfile_SetEntry(profile, 12, 1, 0, 0);
            SceneActorProfile_SetEntry(profile, 4, 3, 0, 0);
            SceneActorProfile_SetEntry(profile, 11, 0, 16, 0);
            break;
        case 5:
            SceneActorProfile_SetEntry(profile, 0, 0, 0, 0);
            SceneActorProfile_SetEntry(profile, 1, 6, 0, 0);
            SceneActorProfile_SetEntry(profile, 5, 12, 0, 0);
            SceneActorProfile_SetEntry(profile, 2, 24, 0, 0);
            SceneActorProfile_SetEntry(profile, 14, 45, 0, 0);
            SceneActorProfile_SetEntry(profile, 15, 47, 0, 0);
            SceneActorProfile_SetEntry(profile, 16, 49, 0, 0);
            SceneActorProfile_SetEntry(profile, 11, 63, 16, 0);
            SceneActorProfile_SetEntry(profile, 6, 69, 0, 0);
            SceneActorProfile_SetEntry(profile, 12, 63, 0, 0);
            SceneActorProfile_SetEntry(profile, 7, 69, 4, 0);
            SceneActorProfile_SetEntry(profile, 3, 81, 0, 0);
            SceneActorProfile_SetEntry(profile, 10, 97, 0, 0);
            SceneActorProfile_SetEntry(profile, 17, 87, 0, 0);
            SceneActorProfile_SetEntry(profile, 18, 89, 0, 0);
            SceneActorProfile_SetEntry(profile, 13, 91, 0, 0);
            break;
        case 6:
            SceneActorProfile_SetEntry(profile, 0, 2, 0, 0);
            SceneActorProfile_SetEntry(profile, 1, 8, 0, 0);
            SceneActorProfile_SetEntry(profile, 5, 14, 0, 0);
            SceneActorProfile_SetEntry(profile, 2, 27, 0, 0);
            SceneActorProfile_SetEntry(profile, 14, 51, 0, 0);
            SceneActorProfile_SetEntry(profile, 15, 53, 0, 0);
            SceneActorProfile_SetEntry(profile, 16, 55, 0, 0);
            SceneActorProfile_SetEntry(profile, 11, 65, 16, 0);
            SceneActorProfile_SetEntry(profile, 6, 73, 0, 0);
            SceneActorProfile_SetEntry(profile, 12, 65, 0, 0);
            SceneActorProfile_SetEntry(profile, 7, 73, 4, 0);
            SceneActorProfile_SetEntry(profile, 3, 83, 0, 0);
            SceneActorProfile_SetEntry(profile, 10, 97, 0, 0);
            SceneActorProfile_SetEntry(profile, 19, 105, 0, 0);
            SceneActorProfile_SetEntry(profile, 20, 107, 0, 0);
            SceneActorProfile_SetEntry(profile, 21, 109, 0, 0);
            SceneActorProfile_SetEntry(profile, 22, 111, 0, 0);
            SceneActorProfile_SetEntry(profile, 23, 113, 0, 0);
            SceneActorProfile_SetEntry(profile, 17, 87, 0, 0);
            SceneActorProfile_SetEntry(profile, 18, 89, 0, 0);
            SceneActorProfile_SetEntry(profile, 13, 93, 0, 0);
            break;
        case 7:
            SceneActorProfile_SetEntry(profile, 0, 4, 0, 0);
            SceneActorProfile_SetEntry(profile, 1, 10, 0, 0);
            SceneActorProfile_SetEntry(profile, 5, 16, 0, 0);
            SceneActorProfile_SetEntry(profile, 2, 33, 0, 0);
            SceneActorProfile_SetEntry(profile, 14, 57, 0, 0);
            SceneActorProfile_SetEntry(profile, 15, 59, 0, 0);
            SceneActorProfile_SetEntry(profile, 16, 61, 0, 0);
            SceneActorProfile_SetEntry(profile, 11, 67, 16, 0);
            SceneActorProfile_SetEntry(profile, 6, 77, 0, 0);
            SceneActorProfile_SetEntry(profile, 7, 79, 4, 0);
            SceneActorProfile_SetEntry(profile, 12, 67, 0, 0);
            SceneActorProfile_SetEntry(profile, 3, 85, 0, 0);
            SceneActorProfile_SetEntry(profile, 10, 97, 0, 0);
            SceneActorProfile_SetEntry(profile, 19, 18, 0, 0);
            SceneActorProfile_SetEntry(profile, 20, 20, 0, 0);
            SceneActorProfile_SetEntry(profile, 21, 22, 0, 0);
            SceneActorProfile_SetEntry(profile, 22, 117, 0, 0);
            SceneActorProfile_SetEntry(profile, 23, 119, 0, 0);
            SceneActorProfile_SetEntry(profile, 17, 87, 0, 0);
            SceneActorProfile_SetEntry(profile, 18, 89, 0, 0);
            SceneActorProfile_SetEntry(profile, 13, 95, 0, 0);
            break;
        case 8:
            SceneActorProfile_SetEntry(profile, 0, 121, 0, 0);
            SceneActorProfile_SetEntry(profile, 1, 6, 0, 0);
            SceneActorProfile_SetEntry(profile, 5, 12, 0, 0);
            SceneActorProfile_SetEntry(profile, 2, 24, 0, 0);
            SceneActorProfile_SetEntry(profile, 14, 45, 0, 0);
            SceneActorProfile_SetEntry(profile, 15, 47, 0, 0);
            SceneActorProfile_SetEntry(profile, 16, 49, 0, 0);
            SceneActorProfile_SetEntry(profile, 11, 63, 16, 0);
            SceneActorProfile_SetEntry(profile, 6, 69, 0, 0);
            SceneActorProfile_SetEntry(profile, 12, 63, 0, 0);
            SceneActorProfile_SetEntry(profile, 7, 69, 4, 0);
            SceneActorProfile_SetEntry(profile, 3, 81, 0, 0);
            SceneActorProfile_SetEntry(profile, 10, 97, 0, 0);
            SceneActorProfile_SetEntry(profile, 17, 87, 0, 0);
            SceneActorProfile_SetEntry(profile, 18, 89, 0, 0);
            SceneActorProfile_SetEntry(profile, 13, 91, 0, 0);
            break;
        case 9:
            SceneActorProfile_SetEntry(profile, 14, 0, 0, 0);
            SceneActorProfile_SetEntry(profile, 15, 2, 0, 0);
            SceneActorProfile_SetEntry(profile, 16, 1, 0, 0);
            SceneActorProfile_SetEntry(profile, 17, 3, 0, 0);
            SceneActorProfile_SetEntry(profile, 18, 4, 0, 0);
            break;
        case 10:
            SceneActorProfile_SetEntry(profile, 0, 0, 0, 0);
            SceneActorProfile_SetEntry(profile, 14, 2, 0, 0);
            SceneActorProfile_SetEntry(profile, 15, 3, 0, 0);
            SceneActorProfile_SetEntry(profile, 16, 1, 0, 0);
            SceneActorProfile_SetEntry(profile, 3, 4, 0, 0);
            SceneActorProfile_SetEntry(profile, 10, 4, 0, 0);
            break;
        case 11:
            break;
        case 12:
            SceneActorProfile_SetEntry(profile, 0, 1, 0, 0);
            SceneActorProfile_SetEntry(profile, 1, 1, 0, 0);
            SceneActorProfile_SetEntry(profile, 3, 1, 0, 0);
            SceneActorProfile_SetEntry(profile, 10, 1, 0, 0);
            break;
        case 13:
            SceneActorProfile_SetEntry(profile, 0, 1, 0, 1);
            SceneActorProfile_SetEntry(profile, 1, 1, 0, 1);
            SceneActorProfile_SetEntry(profile, 3, 1, 0, 1);
            SceneActorProfile_SetEntry(profile, 10, 1, 0, 1);
            break;
        case 14:
            SceneActorProfile_SetEntry(profile, 14, 0, 0, 0);
            SceneActorProfile_SetEntry(profile, 20, 8, 0, 0);
            SceneActorProfile_SetEntry(profile, 15, 1, 0, 0);
            SceneActorProfile_SetEntry(profile, 16, 2, 0, 0);
            SceneActorProfile_SetEntry(profile, 3, 7, 0, 0);
            SceneActorProfile_SetEntry(profile, 5, 3, 0, 0);
            SceneActorProfile_SetEntry(profile, 17, 4, 0, 0);
            SceneActorProfile_SetEntry(profile, 18, 5, 0, 0);
            SceneActorProfile_SetEntry(profile, 19, 6, 0, 0);
            break;
        case 15:
            break;
        case 16:
            break;
        case 17:
            SceneActorProfile_SetEntry(profile, 0, 0, 0, 0);
            SceneActorProfile_SetEntry(profile, 1, 0, 0, 0);
            SceneActorProfile_SetEntry(profile, 2, 0, 0, 0);
            SceneActorProfile_SetEntry(profile, 3, 5, 0, 0);
            SceneActorProfile_SetEntry(profile, 14, 1, 0, 0);
            SceneActorProfile_SetEntry(profile, 15, 2, 0, 0);
            SceneActorProfile_SetEntry(profile, 16, 3, 0, 0);
            SceneActorProfile_SetEntry(profile, 11, 0, 0, 0);
            SceneActorProfile_SetEntry(profile, 10, 6, 0, 0);
            break;
        case 18:
            SceneActorProfile_SetEntry(profile, 0, 0, 0, 0);
            SceneActorProfile_SetEntry(profile, 1, 0, 0, 0);
            SceneActorProfile_SetEntry(profile, 3, 1, 0, 0);
            SceneActorProfile_SetEntry(profile, 15, 2, 0, 0);
            SceneActorProfile_SetEntry(profile, 16, 3, 0, 0);
            SceneActorProfile_SetEntry(profile, 17, 4, 0, 0);
            SceneActorProfile_SetEntry(profile, 18, 5, 0, 0);
            SceneActorProfile_SetEntry(profile, 10, 1, 0, 0);
            break;
        case 19:
            SceneActorProfile_SetEntry(profile, 0, 4, 0, 0);
            SceneActorProfile_SetEntry(profile, 1, 0, 0, 0);
            SceneActorProfile_SetEntry(profile, 2, 2, 0, 0);
            SceneActorProfile_SetEntry(profile, 3, 3, 0, 0);
            SceneActorProfile_SetEntry(profile, 14, 1, 0, 0);
            SceneActorProfile_SetEntry(profile, 15, 6, 0, 0);
            SceneActorProfile_SetEntry(profile, 16, 7, 0, 0);
            SceneActorProfile_SetEntry(profile, 17, 5, 0, 0);
            SceneActorProfile_SetEntry(profile, 10, 3, 0, 0);
            break;
        case 20:
            SceneActorProfile_SetEntry(profile, 0, 0, 0, 0);
            SceneActorProfile_SetEntry(profile, 1, 0, 0, 0);
            SceneActorProfile_SetEntry(profile, 3, 3, 0, 0);
            SceneActorProfile_SetEntry(profile, 10, 3, 0, 0);
            SceneActorProfile_SetEntry(profile, 14, 1, 0, 0);
            SceneActorProfile_SetEntry(profile, 5, 2, 0, 0);
            break;
        case 21:
            SceneActorProfile_SetEntry(profile, 0, 0, 0, 0);
            SceneActorProfile_SetEntry(profile, 14, 1, 0, 0);
            SceneActorProfile_SetEntry(profile, 11, 2, 16, 0);
            SceneActorProfile_SetEntry(profile, 1, 3, 0, 0);
            SceneActorProfile_SetEntry(profile, 2, 13, 0, 0);
            SceneActorProfile_SetEntry(profile, 15, 5, 0, 0);
            SceneActorProfile_SetEntry(profile, 5, 6, 0, 0);
            SceneActorProfile_SetEntry(profile, 16, 7, 0, 0);
            SceneActorProfile_SetEntry(profile, 17, 8, 0, 0);
            SceneActorProfile_SetEntry(profile, 3, 9, 0, 0);
            SceneActorProfile_SetEntry(profile, 6, 10, 0, 0);
            SceneActorProfile_SetEntry(profile, 7, 11, 0, 0);
            SceneActorProfile_SetEntry(profile, 10, 12, 0, 0);
            break;
        case 22:
            SceneActorProfile_SetEntry(profile, 0, 0, 0, 0);
            SceneActorProfile_SetEntry(profile, 14, 2, 0, 0);
            SceneActorProfile_SetEntry(profile, 15, 1, 0, 0);
            SceneActorProfile_SetEntry(profile, 1, 3, 0, 0);
            SceneActorProfile_SetEntry(profile, 3, 4, 0, 0);
            SceneActorProfile_SetEntry(profile, 10, 5, 0, 0);
            SceneActorProfile_SetEntry(profile, 16, 8, 0, 0);
            SceneActorProfile_SetEntry(profile, 17, 9, 0, 0);
            SceneActorProfile_SetEntry(profile, 18, 9, 4, 0);
            break;
        case 23:
            SceneActorProfile_SetEntry(profile, 0, 0, 0, 0);
            SceneActorProfile_SetEntry(profile, 1, 0, 0, 0);
            SceneActorProfile_SetEntry(profile, 2, 0, 0, 0);
            SceneActorProfile_SetEntry(profile, 3, 2, 0, 0);
            SceneActorProfile_SetEntry(profile, 14, 0, 0, 0);
            SceneActorProfile_SetEntry(profile, 15, 3, 0, 0);
            SceneActorProfile_SetEntry(profile, 11, 0, 0, 0);
            SceneActorProfile_SetEntry(profile, 10, 1, 0, 0);
            break;
        case 24:
            SceneActorProfile_SetEntry(profile, 14, 0, 0, 0);
            SceneActorProfile_SetEntry(profile, 15, 1, 0, 0);
            SceneActorProfile_SetEntry(profile, 16, 2, 0, 0);
            SceneActorProfile_SetEntry(profile, 3, 7, 0, 0);
            SceneActorProfile_SetEntry(profile, 5, 3, 0, 0);
            SceneActorProfile_SetEntry(profile, 17, 4, 0, 0);
            SceneActorProfile_SetEntry(profile, 18, 5, 0, 0);
            SceneActorProfile_SetEntry(profile, 19, 6, 0, 0);
            SceneActorProfile_SetEntry(profile, 20, 8, 0, 0);
            SceneActorProfile_SetEntry(profile, 10, 8, 0, 0);
            SceneActorProfile_SetEntry(profile, 21, 9, 0, 0);
            break;
        case 25:
            SceneActorProfile_SetEntry(profile, 0, 0, 0, 0);
            SceneActorProfile_SetEntry(profile, 1, 2, 0, 0);
            SceneActorProfile_SetEntry(profile, 3, 8, 0, 0);
            SceneActorProfile_SetEntry(profile, 10, 10, 0, 0);
            SceneActorProfile_SetEntry(profile, 5, 6, 0, 0);
            SceneActorProfile_SetEntry(profile, 14, 4, 0, 0);
            SceneActorProfile_SetEntry(profile, 15, 13, 0, 0);
            SceneActorProfile_SetEntry(profile, 16, 12, 0, 0);
            SceneActorProfile_SetEntry(profile, 2, 0, 0, 0);
            SceneActorProfile_SetEntry(profile, 11, 0, 16, 0);
            SceneActorProfile_SetEntry(profile, 6, 15, 0, 0);
            SceneActorProfile_SetEntry(profile, 12, 0, 0, 0);
            SceneActorProfile_SetEntry(profile, 7, 0, 0, 0);
            SceneActorProfile_SetEntry(profile, 9, 10, 4, 0);
            break;
        default:
            return -1;
    }
    return profile->id;
}
