#include "global.h"
struct Slot {
    u32 flags;
    u8 pad[0x34];
};
void sub_081FDBC4(u8 *p)
{
    s32 i;
    for (i = 0; i < 12; i++)
        ((struct Slot *)(p + 0x100))[i].flags &= ~1;
    *(u16 *)(p + 0xfa) = 0;
}