#include "global.h"
extern u8 *gGameState;
// Sets bit i of the 64-bit flag set at gGameState + 0x720.
void Inventory_SetFlag(u32 i)
{
    if ((s32)i > 0x1f) {
        *(u32 *)(gGameState + 0x724) |= 1 << (i - 0x20);
    } else {
        *(u32 *)(gGameState + 0x720) |= 1 << i;
    }
}
