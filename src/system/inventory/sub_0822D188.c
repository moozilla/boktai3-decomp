#include "global.h"
s32 Inventory_GetKeyItemId(s32);
void Inventory_SwapValuableSlots(s32, s32);
/* Close the gaps in the 16 slots from `start`, then sort the used slots. */
void Inventory_SortValuables(s32 start)
{
    s32 used = 0;
    s32 gap = 0;
    s32 i, j;
    for (i = start; i <= 15; i++) {
        if (Inventory_GetKeyItemId(i) < 0)
            gap++;
        else {
            used++;
            if (gap > 0)
                Inventory_SwapValuableSlots(i, i - gap);
        }
    }
    if (used > 1) {
        for (i = 0; i < used - 1; i++) {
            for (j = i; j < used; j++) {
                if (Inventory_GetKeyItemId(i) > Inventory_GetKeyItemId(j))
                    Inventory_SwapValuableSlots(i, j);
            }
        }
    }
}
