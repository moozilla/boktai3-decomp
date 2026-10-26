#include "bouncing_object.h"
static inline u8 Occupied(const u32 *mask, u32 bit)
{
    if (*mask & bit)
        return TRUE;
    return FALSE;
}
static inline u32 Flags(const u32 *flags, u32 mask)
{
    return *flags & mask;
}
s32 BouncingObject_UpdatePool(struct BouncingPool *input)
{
    struct BouncingPool *pool = input;
    struct BouncingObject *slots = pool->slots;
    s32 i;
    u32 offset = (u32) & ((struct BouncingPool *)0)->used;
    u32 *guard;
    guard = (void *)((u32)pool + offset);
    offset = 0;
    if (!*guard) {
        u32 stoppingOffset;
        u32 *stopping;
        Task_Kill(pool);
        stoppingOffset = (u32) & ((struct BouncingPool *)0)->stopping;
        stopping = (void *)((u32)pool + stoppingOffset);
        *stopping = 1;
    } else {
        u32 mask = 0x400;
        if (!((gEngineFlags | gScenePauseFlags) & mask)) {
            u32 blockedMask = 4;
            u32 blocked;
            blocked = gUnk_02000560 & blockedMask;
            if (!blocked) {
                u32 *used;
                void **table;
                i = 0;
                {
                    u32 usedOffset = (u32) & ((struct BouncingPool *)0)->used;
                    used = (void *)((u32)pool + usedOffset);
                }
                {
                    void **low = gUnk_08605B6C;
                    table = low;
                }
                do {
                    if (Occupied(used, 1u << i)) {
                        struct BouncingObject *slot =
                            (void *)((u32)i * sizeof(*slots) + (u32)slots);
                        sub_08249240(pool, slot, table[slot->state]);
                    }
                    i++;
                } while (i < 12);
            }
        }
    }
    return offset;
}
