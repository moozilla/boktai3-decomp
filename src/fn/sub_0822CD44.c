#include "global.h"
extern u8 *gUnk_02000710;
/* Set the high bit (0x8000) of game-state word 0x100 + 2 * index. */
void Inventory_SetItemOverrideFlag(s32 index)
{
    ((s16 *)(gUnk_02000710 + 0x100))[index] |= 0x8000;
}
