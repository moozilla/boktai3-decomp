#include "global.h"
extern u8 *gGameState;
/* Store the low 15 bits of game-state word 0x100 + 2 * index, keeping bit 15. */
void Inventory_SetItemStateValue(s32 index, u32 value)
{
    ((s16 *)(gGameState + 0x100))[index] =
        (value & 0x7fff) | (((s16 *)(gGameState + 0x100))[index] & 0x8000);
}
