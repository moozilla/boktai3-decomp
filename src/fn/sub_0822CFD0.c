#include "global.h"
s32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
void Inventory_RemoveItem(u32);
u32 Inventory_RemoveAllItemsOfKindFromScript(void)
{
    u32 a;
    s32 i;
    if (Script_SeekToKeyword(0x69) == 0)
        return 0;
    a = Script_GetValue();
    i = 0xf;
    do {
        Inventory_RemoveItem(a);
        i--;
    } while (i >= 0);
}
