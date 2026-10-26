#include "global.h"
#include "ram.h"
u32 Inventory_GetUsePermissionFlag(u32 bit)
{
    return gInventoryUseFlags & (1 << bit);
}
