// MATCHING_CONCESSION: register-pinning
#include "global.h"
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
    if (!gUnk_020004B4 || gUnk_020004B4->busy)
        return 0;
    return gUnk_020004B4->slotView;
}
s32 SelectionSlots_ClearValue(void *unused, s32 value)
{
    s8 *slot;
    s32 i;
    if (!SelectionSlots_GetView())
        return -1;
    {
        register u32 m asm("r1") = 0xFF;
        slot = gUnk_020004B4->values;
        i = 4;

        do {
            if (*slot == value)
                *slot |= m;
            slot++;
            i--;
        } while (i >= 0);
    }
    return 0;
}
