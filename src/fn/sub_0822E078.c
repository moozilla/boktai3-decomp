#include "global.h"
extern u8 *gUnk_02000710;
// Sets bit i of the 64-bit flag set at gUnk_02000710 + 0x720.
void Inventory_SetFlag(u32 i)
{
    if ((s32)i > 0x1f) {
        *(u32 *)(gUnk_02000710 + 0x724) |= 1 << (i - 0x20);
    } else {
        *(u32 *)(gUnk_02000710 + 0x720) |= 1 << i;
    }
}
