#include "global.h"
s32 Inventory_GetConsumableId(s32);
void Inventory_SwapSlots(s32, s32);
void GameState_SortConsumableSlots(s32 first)
{
    s32 count = 0, gaps = 0, i, j;
    for (i = 0; i < 16; i++) {
        if (Inventory_GetConsumableId(first + i) < 0)
            gaps++;
        else {
            count++;
            if (gaps > 0)
                Inventory_SwapSlots(first + i, first + i - gaps);
        }
    }
    if (count > 1) {
        for (i = 0; i < count - 1; i++)
            for (j = i; j < count; j++) {
                if (Inventory_GetConsumableId(first + i) > Inventory_GetConsumableId(first + j))
                    Inventory_SwapSlots(first + i, first + j);
            }
    }
}
