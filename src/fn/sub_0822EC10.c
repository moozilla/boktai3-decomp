#include "global.h"
s32 SolarFruit_GetInventorySlot(u8 *);
void sub_0822EB14(u8 *, u8 *);
void SolarFruit_SortInventorySlots(u8 *p)
{
    s32 count = 0, gaps = 0, i, j;
    for (i = 0; i < 8; i++) {
        u8 *q = p + i;
        if (SolarFruit_GetInventorySlot(q) < 0)
            gaps++;
        else {
            count++;
            if (gaps > 0)
                sub_0822EB14(q, q - gaps);
        }
    }
    if (count > 1) {
        for (i = 0; i < count - 1; i++)
            for (j = i; j < count; j++) {
                if (SolarFruit_GetInventorySlot(p + i) > SolarFruit_GetInventorySlot(p + j))
                    sub_0822EB14(p + i, p + j);
            }
    }
}
