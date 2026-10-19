
#ifdef NONMATCHING
// fuzzy: 91.7%
// foundry draft (worker best 91.7%); what differs is mostly register allocation and instruction order
#define EffectPool_CheckCapacity   sub_081144A8
#define EffectPool_MarkLinkedSlots SlotRegistry_ClaimMatchingSlot
#include "global.h"
struct EffectPoolSlot {
    u8 pad0[0xc];
    u16 group;
    u8 padE;
    u8 active;
    u8 pad10[0xd8 - 0x10];
};
struct EffectPool {
    u8 task[0xd8];
    struct EffectPoolSlot slots[16];
};
void Memory_Clear(void *, u32);
void SlotRegistry_ClaimMatchingSlot(struct EffectPool *, s32, u32, u8 *);
s32 sub_081144A8(struct EffectPool *pool, s32 group, s32 required, s32 allowGroups)
{
    u8 visited[16];
    s32 groups = 0;
    s32 freeSlots = 0;
    s32 i;
    group &= 0xff;
    Memory_Clear(visited, 16);
    for (i = 0; i < 16; i++) {
        if (!pool->slots[i].active)
            freeSlots++;
        else if ((u8)pool->slots[i].group == group && !visited[i]) {
            visited[i] = 1;
            SlotRegistry_ClaimMatchingSlot(pool, i + 1, pool->slots[i].group, visited);
            groups++;
        }
    }
    if (!allowGroups && groups > 2)
        return 1;
    if (freeSlots < required)
        return 1;
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081144A8);
#endif
