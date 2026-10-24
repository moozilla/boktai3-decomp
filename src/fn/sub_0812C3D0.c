#include "global.h"

struct SceneSlot {
    u8 unknown[0xbb];
    s8 index;
    u8 padBC[0xc0 - 0xbc];
};
struct SceneSlotPool {
    u8 unknown[0x38];
    u32 occupied;
    struct SceneSlot slots[8];
};
static inline u8 SceneSlot_IsOccupied(struct SceneSlotPool *pool, u32 index)
{
    if ((1 << index) & pool->occupied)
        return 1;
    return 0;
}
struct SceneSlot *sub_0812C3D0(struct SceneSlotPool *pool)
{
    s32 i;
    for (i = 0; i < 8; i++) {
        if (!SceneSlot_IsOccupied(pool, i)) {
            pool->occupied |= 1 << i;
            {
                struct SceneSlot *slot = &pool->slots[i];
                slot->index = i;
                return slot;
            }
        }
    }
    return 0;
}
