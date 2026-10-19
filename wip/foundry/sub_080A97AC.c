#define SceneActorSlotGroup_SelectMarkedEntry sub_080A97AC
#include "scene_actor_reset.h"
struct SceneActorSlotEntry {
    u8 unknown0[0x1C];
    struct ContactPosition position;
    u8 unknown24[0xF8 - 0x24];
    u16 slot;
    u8 unknownFA[0xB];
    u8 flags;
    u8 unknown106[0x22];
};
struct SceneActorSlotGroup {
    u8 unknown0[0x3D4];
    struct SceneActorSlotEntry entries[4];
    u8 unknown874[0x20];
    struct ContactPosition selectedPosition;
    u8 unknown89C[0x18];
    struct SceneActorSlotEntry *selectedEntry;
    u8 unknown8B8[0x18];
    u16 flags;
    u8 unknown8D2[8];
    s16 count;
};
static inline u8 HasEntryFlags(struct SceneActorSlotGroup *group, u32 offset, u32 mask)
{
    struct SceneActorSlotEntry *entry = (void *)((u8 *)group + offset);
    entry = (void *)((u8 *)entry + 0x3D4);
    if (entry->flags & mask)
        return 1;
    return 0;
}
static inline u8 HasGroupFlags(struct SceneActorSlotGroup *group, u32 mask)
{
    if (group->flags & mask)
        return 1;
    return 0;
}
void sub_080A97AC(struct SceneActorRegistryResetView *p)
{
    struct SceneActorSlotGroup *group = p->context;
    s32 selected = 255;
    s32 i;
    s32 count;
    i = 0;
    count = group->count;
    if (i < count) {
        s32 offset = 0;
        do {
            if (HasEntryFlags(group, offset, 4)) {
                selected = i;
                break;
            }
            offset += sizeof(struct SceneActorSlotEntry);
            i++;
        } while (i < count);
    }
    if (selected != 255 && HasGroupFlags(group, 1)) {
        u32 offset = 0x894;
        struct ContactPosition *position = (void *)((u8 *)group + offset);
        *position = group->entries[selected].position;
        offset += 0x20;
        *(struct SceneActorSlotEntry **)((u8 *)group + offset) = &group->entries[selected];
    }
}
