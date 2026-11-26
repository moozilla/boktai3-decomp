#include "global.h"
struct Slot {
    u8 state;
    u8 mode;
    u8 pad[0x82];
    u32 timer;
    u8 pad2[600 - 0x88];
};

s32 sub_081F2B70(u8 *p)
{
    struct Slot *slot = (struct Slot *)(p + 0x20);
    s32 i;
    for (i = 0; i <= 3; i++, slot++) {
        if ((*(u32 *)(p + 0x18) & (1 << i)) && slot->mode == 0) {
            slot->mode = 2;
            slot->state = 1;
            slot->timer = 0;
        }
    }
}