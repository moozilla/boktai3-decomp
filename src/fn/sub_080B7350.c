#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_profile.h"
static inline void SetEntry(u32 value, u32 flags, struct SceneActorProfileEntry *entry)
{
    entry->value = value;
    entry->flags = flags;
}
// confidence=low: writes the animation/action profile shared by the three Skeleton categories
void Skeleton_InitializeCategoryProfile(struct SceneActorRegistryResetView *actor)
{
    s32 category = actor->category;
    struct SceneActorProfileEntry *entry =
        (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, 0);
    if (!entry)
        return;
    switch (category) {
        case 5:
            SetEntry(0x0, 0x0, entry + 0);
            SetEntry(0x6, 0x0, entry + 1);
            SetEntry(0xC, 0x0, entry + 5);
            SetEntry(0x18, 0x0, entry + 2);
            SetEntry(0x2D, 0x0, entry + 14);
            SetEntry(0x2F, 0x0, entry + 15);
            SetEntry(0x31, 0x0, entry + 16);
            SetEntry(0x3F, 0x10, entry + 11);
            SetEntry(0x45, 0x0, entry + 6);
            SetEntry(0x3F, 0x0, entry + 12);
            SetEntry(0x45, 0x4, entry + 7);
            SetEntry(0x51, 0x0, entry + 3);
            SetEntry(0x61, 0x0, entry + 10);
            SetEntry(0x57, 0x0, entry + 17);
            SetEntry(0x59, 0x0, entry + 18);
            SetEntry(0x5B, 0x0, entry + 13);
            break;
        case 6:
            SetEntry(0x2, 0x0, entry + 0);
            SetEntry(0x8, 0x0, entry + 1);
            SetEntry(0xE, 0x0, entry + 5);
            SetEntry(0x1B, 0x0, entry + 2);
            SetEntry(0x33, 0x0, entry + 14);
            SetEntry(0x35, 0x0, entry + 15);
            SetEntry(0x37, 0x0, entry + 16);
            SetEntry(0x41, 0x10, entry + 11);
            SetEntry(0x49, 0x0, entry + 6);
            SetEntry(0x41, 0x0, entry + 12);
            SetEntry(0x49, 0x4, entry + 7);
            SetEntry(0x53, 0x0, entry + 3);
            SetEntry(0x61, 0x0, entry + 10);
            SetEntry(0x69, 0x0, entry + 19);
            SetEntry(0x6B, 0x0, entry + 20);
            SetEntry(0x6D, 0x0, entry + 21);
            SetEntry(0x6F, 0x0, entry + 22);
            SetEntry(0x71, 0x0, entry + 23);
            SetEntry(0x57, 0x0, entry + 17);
            SetEntry(0x59, 0x0, entry + 18);
            SetEntry(0x5D, 0x0, entry + 13);
            break;
        case 7:
            SetEntry(0x4, 0x0, entry + 0);
            SetEntry(0xA, 0x0, entry + 1);
            SetEntry(0x10, 0x0, entry + 5);
            SetEntry(0x21, 0x0, entry + 2);
            SetEntry(0x39, 0x0, entry + 14);
            SetEntry(0x3B, 0x0, entry + 15);
            SetEntry(0x3D, 0x0, entry + 16);
            SetEntry(0x43, 0x10, entry + 11);
            SetEntry(0x4D, 0x0, entry + 6);
            SetEntry(0x4F, 0x4, entry + 7);
            SetEntry(0x43, 0x0, entry + 12);
            SetEntry(0x55, 0x0, entry + 3);
            SetEntry(0x61, 0x0, entry + 10);
            SetEntry(0x12, 0x0, entry + 19);
            SetEntry(0x14, 0x0, entry + 20);
            SetEntry(0x16, 0x0, entry + 21);
            SetEntry(0x75, 0x0, entry + 22);
            SetEntry(0x77, 0x0, entry + 23);
            SetEntry(0x57, 0x0, entry + 17);
            SetEntry(0x59, 0x0, entry + 18);
            SetEntry(0x5F, 0x0, entry + 13);
            break;
    }
}
