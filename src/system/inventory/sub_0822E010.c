#include "global.h"
extern u8 *gGameState;
void Inventory_ResetEquipmentAssignments(void)
{
    s32 i = 0;
    do {
        *(s16 *)(gGameState + i * 2 + 0x68) = -1;
        i++;
    } while (i <= 7);
    *(u16 *)(gGameState + 0x50) = 8;
}
