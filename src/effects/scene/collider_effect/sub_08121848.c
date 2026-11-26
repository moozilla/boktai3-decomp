#include "global.h"
struct Slot {
    u8 pad[0x9b];
    u8 index;
    u8 pad2[8];
};
struct Manager {
    u8 pad[0x38];
    u32 used;
    struct Slot slots[4];
};
static inline u8 IsUsed(struct Manager *p, u32 index)
{
    if (p->used & (1U << index))
        return 1;
    return 0;
}
struct Slot *sub_08121848(struct Manager *p)
{
    s32 i;
    for (i = 0; i < 4; i++) {
        if (!IsUsed(p, i)) {
            struct Slot *slot = &p->slots[i];
            p->used |= 1U << i;
            slot->index = i;
            return slot;
        }
    }
    return 0;
}
