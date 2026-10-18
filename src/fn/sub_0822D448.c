#include "global.h"
u32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
void Inventory_ResetConsumables(void);
void Inventory_ResetValuables(void);
void Inventory_AddItem(s32, s32);
void Inventory_ResetItemsFromScript(void)
{
    s32 n;
    Inventory_ResetConsumables();
    Inventory_ResetValuables();
    if (Script_SeekToKeyword(0x6e))
        n = Script_GetValue();
    else
        n = 0;
    if (n > 0 && Script_SeekToKeyword(0x69)) {
        s32 i;
        for (i = 0; i < n; i++)
            Inventory_AddItem(Script_GetValue(), 0);
    }
}
