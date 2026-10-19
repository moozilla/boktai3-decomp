#include "global.h"
#ifdef NONMATCHING
// fuzzy: 90.0%
// foundry draft (worker best 90.0%); what differs is mostly register allocation and instruction order
#define SelectionSlots_ClearValue sub_08109040
#define gSelectionSlots gUnk_020004B4
struct SelectionSlots {
    u8 pad0[0x53];
    u8 busy;
    u8 pad54[0xE0 - 0x54];
    u8 slotView[0x84];
    s8 values[5];
};
extern struct SelectionSlots *gUnk_020004B4;
static inline void *SelectionSlots_GetView(void)
{
    if (!gUnk_020004B4 || gUnk_020004B4->busy) return 0;
    return gUnk_020004B4->slotView;
}
s32 sub_08109040(void *unused, s32 value)
{
    s8 *slot;
    s32 i;
    if (!SelectionSlots_GetView()) return -1;
    slot = gUnk_020004B4->values;
    i = 4;
    do {
        if (*slot == value) *slot |= 0xFF;
        slot++;
        i--;
    } while (i >= 0);
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08109040);
#endif
