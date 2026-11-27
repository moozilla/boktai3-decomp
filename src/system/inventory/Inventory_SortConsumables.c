#include "global.h"
s32 Inventory_GetItemId(u8 *);
void sub_0822D080(u8 *, u8 *);
void Inventory_SortConsumables(u8 *p)
{
    s32 count = 0, gaps = 0, i, j;
    for (i = 0; i < 16; i++) {
        u8 *q = p + i;
        if (Inventory_GetItemId(q) < 0)
            gaps++;
        else {
            count++;
            if (gaps > 0)
                sub_0822D080(q, q - gaps);
        }
    }
    if (count > 1) {
        for (i = 0; i < count - 1; i++)
            for (j = i; j < count; j++) {
                if (Inventory_GetItemId(p + i) > Inventory_GetItemId(p + j))
                    sub_0822D080(p + i, p + j);
            }
    }
}
