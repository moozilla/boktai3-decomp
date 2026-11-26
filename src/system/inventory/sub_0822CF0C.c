#include "global.h"
s32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
u32 Inventory_AddItem(u32, u32);
u32 Inventory_AddItemFromScript(void)
{
    u32 a, b;
    if (Script_SeekToKeyword(0x69) == 0)
        return 0;
    a = Script_GetValue();
    if (Script_SeekToKeyword(0x70))
        b = Script_GetValue();
    else
        b = 0;
    return Inventory_AddItem(a, b);
}
