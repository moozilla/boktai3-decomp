#include "global.h"
s32 SolarFruit_GetInventorySlot(s32);
void sub_0822EB14(s32, s32);
void SolarFruit_SortInventorySlots(s32 first)
{
    s32 count = 0, gaps = 0, i, j;

    for (i = 0; i < 8; i++) {
        if (SolarFruit_GetInventorySlot(first + i) < 0)
            gaps++;
        else {
            count++;
            if (gaps > 0)
                sub_0822EB14(first + i, first + i - gaps);
        }
    }
    if (count > 1) {
        for (i = 0; i < count - 1; i++)
            for (j = i; j < count; j++) {
                if (SolarFruit_GetInventorySlot(first + i) > SolarFruit_GetInventorySlot(first + j))
                    sub_0822EB14(first + i, first + j);
            }
    }
}
