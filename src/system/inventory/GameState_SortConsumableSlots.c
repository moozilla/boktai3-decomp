#include "global.h"
s32 Inventory_GetConsumableId(u8 *);
void Inventory_SwapSlots(u8 *, u8 *);
void GameState_SortConsumableSlots(u8 *p)
{
    s32 count = 0, gaps = 0, i, j;
    for (i = 0; i < 16; i++) {
        u8 *q = p + i;
        if (Inventory_GetConsumableId(q) < 0)
            gaps++;
        else {
            count++;
            if (gaps > 0)
                Inventory_SwapSlots(q, q - gaps);
        }
    }
    if (count > 1) {
        for (i = 0; i < count - 1; i++)
            for (j = i; j < count; j++) {
                if (Inventory_GetConsumableId(p + i) > Inventory_GetConsumableId(p + j))
                    Inventory_SwapSlots(p + i, p + j);
            }
    }
}
