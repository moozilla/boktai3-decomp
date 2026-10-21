#include "global.h"
#ifdef NONMATCHING
// fuzzy: 96.4%
// open: mask address (manager+0x920) must be rematerialized, not CSE with the occupancy test
struct SpriteNotice {
    u8 unk0[0x42];
    u8 state;
    u8 unk43[5];
};
struct SpriteNoticeManager {
    u8 task[0x18];
    void *animationData, *spriteData;
    struct SpriteNotice slots[32];
    u32 occupiedMask;
};
extern void *gUnk_08610728[];
s32 sub_08249240(void *, void *, void *);
static inline u8 Occupied(u32 *mask, s32 i)
{
    if (*mask & (1 << i))
        return TRUE;
    return FALSE;
}
s32 SpriteNotice_TickManager(struct SpriteNoticeManager *manager)
{
    struct SpriteNotice *start;
    s32 i;
    u32 *mask;
    void **table;
    u8 *state;
    struct SpriteNotice *slot;
    if (manager->occupiedMask) {
        i = 0;
        start = manager->slots;
        mask = (u32 *)((u32)start + 0x900);
        table = gUnk_08610728;
        state = &manager->slots[0].state;
        slot = start;
        for (; i < 32;) {
            if (Occupied(mask, i))
                sub_08249240(manager, slot, table[*state]);
            state += 0x48;
            slot = (void *)((u32)slot + 0x48);
            i++;
        }
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", SpriteNotice_TickManager);
#endif
