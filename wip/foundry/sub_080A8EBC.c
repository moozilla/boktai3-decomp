#define SceneActorSlotGroup_AssignAvailableSlots sub_080A8EBC
#include "global.h"
struct SceneActorSlotEntry {
    u8 unknown0[0xF8];
    u16 slot;
    u8 unknownFA[0xB];
    u8 flags;
    u8 unknown106[0x22];
};
struct SceneActorSlotGroup {
    u8 unknown0[0x3D4];
    struct SceneActorSlotEntry entries[4];
    u8 unknown874[0x66];
    s16 count;
};
static inline u8 HasSlotFlags(u8 *flags, u32 mask)
{
    if (*flags & mask)
        return 1;
    return 0;
}
void sub_080A8EBC(struct SceneActorSlotGroup *group)
{
    s32 slot = 0;
    s32 i = 0;
    if (i < group->count) {
        struct SceneActorSlotGroup *cursor = group;
        do {
            if (!HasSlotFlags(&group->entries[i].flags, 5)) {
                cursor->entries[0].slot = slot;
                slot++;
                if (slot > 3)
                    slot = 0;
            }
            cursor = (void *)((u8 *)cursor + sizeof(struct SceneActorSlotEntry));
            i++;
        } while (i < group->count);
    }
}
