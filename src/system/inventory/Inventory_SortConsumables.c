#include "global.h"
s32 Inventory_GetItemId(s32);
void sub_0822D080(s32, s32);
void Inventory_SortConsumables(s32 first)
{
    s32 count = 0, gaps = 0, i, j;
    for (i = 0; i < 16; i++) {
        if (Inventory_GetItemId(first + i) < 0)
            gaps++;
        else {
            count++;
            if (gaps > 0)
                sub_0822D080(first + i, first + i - gaps);
        }
    }
    if (count > 1) {
        for (i = 0; i < count - 1; i++)
            for (j = i; j < count; j++) {
                if (Inventory_GetItemId(first + i) > Inventory_GetItemId(first + j))
                    sub_0822D080(first + i, first + j);
            }
    }
}
