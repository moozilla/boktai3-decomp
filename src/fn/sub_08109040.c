#include "global.h"
#include "area_effect_link.h"
struct SelectionSlots {
    u8 pad0[0x53];
    u8 busy;
    u8 pad54[0xE0 - 0x54];
    u8 slotView[0x84];
    s8 values[5];
};
static inline void *SelectionSlots_GetView(void)
{
    if (!((struct SelectionSlots *)gLinkSession) || ((struct SelectionSlots *)gLinkSession)->busy)
        return 0;
    return ((struct SelectionSlots *)gLinkSession)->slotView;
}
s32 SelectionSlots_ClearValue(void *unused, s32 value)
{
    s8 *slot;
    s32 i;
    u32 m;
    if (!SelectionSlots_GetView())
        return -1;
    do {
        m = 0xFF;
    } while (0);
    slot = ((struct SelectionSlots *)gLinkSession)->values;
    i = 4;
    do {
        if (*slot == value) {
            u32 cur = (u8)*slot;
            *slot = cur | m;
        }
        slot++;
        i--;
    } while (i >= 0);
    return 0;
}
