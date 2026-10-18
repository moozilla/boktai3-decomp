#include "global.h"
#define SlotRegistry_ClaimMatchingSlot sub_08114464
struct RegistrySlot {
    u16 id;
    u8 unk2;
    u8 active;
    u8 padding[0xD4];
};
struct SlotRegistry {
    u8 padding[0xE4];
    struct RegistrySlot slots[16];
};
s32 sub_08114464(struct SlotRegistry *registry, s32 start, s32 id, u8 *claimed)
{
    struct RegistrySlot *slot;
    s32 index = start;
    u8 *entry;
    u32 offset;
    if (index <= 15) {
        offset = index * sizeof(struct RegistrySlot) + 0xE4;
        slot = (struct RegistrySlot *)(offset + (u32)registry);
        do {
            if (slot->active) {
                entry = claimed + index;
                if (!*entry && slot->id == id) {
                    *entry = 1;
                    return 1;
                }
            }
            slot++;
            index++;
        } while (index <= 15);
    }
    return 0;
}
