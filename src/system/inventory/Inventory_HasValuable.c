#include "global.h"
u32 Inventory_GetKeyItemId(u32);
u32 Inventory_HasValuable(u32 a)
{
    s32 i;
    for (i = 0; i <= 0xf; i++) {
        if (Inventory_GetKeyItemId(i) == a)
            return 1;
    }
    return 0;
}
