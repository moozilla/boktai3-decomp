#include "global.h"
#ifdef NONMATCHING
// fuzzy: 80.7%
// foundry draft (worker best 80.7%); what differs is mostly register allocation and instruction order
#define SpriteNotice_TickManager sub_08123044
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
s32 sub_08123044(struct SpriteNoticeManager *manager)
{
    struct SpriteNotice *start = manager->slots;
    s32 i;
    u32 *mask;
    void **table;
    u8 *state;
    struct SpriteNotice *slot;
    if (manager->occupiedMask) {
        i = 0;
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
INCLUDE_ASM("asm/nonmatching", sub_08123044);
#endif
