#include "global.h"
extern const u16 gUnk_08E61280[];
u32 Inventory_GetItemUseCondition(u32 index)
{
    return (gUnk_08E61280[index * 8] & 0xF0) >> 4;
}
