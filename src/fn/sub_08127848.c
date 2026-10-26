#include "global.h"
#include "engine.h"

struct StateSlot {
    u8 unknown00[0xc0];
    u8 state;
    u8 unknownC1[0x13];
};
struct StateSlotPool {
    u8 task[0x18];
    struct StateSlot slots[10];
    u32 used;
};
extern u32 gUnk_030053F0, gUnk_02000560;
extern void *gUnk_08610770[];
s32 sub_08249240(void *, void *, void *);
static inline u8 Occupied(const u32 *mask, u32 bit)
{
    if (*mask & bit)
        return TRUE;
    return FALSE;
}
s32 StateSlotPool_Update(struct StateSlotPool *input)
{
    struct StateSlotPool *pool = input;
    struct StateSlot *slots = pool->slots;
    s32 i;
    if (pool->used) {
        u32 mask = 0x400;
        if (!((gEngineFlags | gUnk_030053F0) & mask)) {
            u32 blockedMask = 4;
            u32 blocked = gUnk_02000560 & blockedMask;
            if (!blocked) {
                u32 *used;
                void **table;
                i = 0;
                {
                    u16 usedOffset = (u32) & ((struct StateSlotPool *)0)->used;
                    used = (void *)((u32)slots + usedOffset - 0x18);
                }
                table = gUnk_08610770;
                do {
                    if (Occupied(used, 1u << i)) {
                        struct StateSlot *slot = (void *)((u32)i * sizeof(*slots) + (u32)slots);
                        sub_08249240(pool, slot, table[slot->state]);
                    }
                    i++;
                } while (i < 10);
            }
        }
    }
    return 0;
}
