#include "engine/engine.h"
#include "global.h"
u32 Inventory_AddItem(u32, u32);
u32 Inventory_AddItemFromScript(void)
{
    u32 a, b;
    if (Script_SeekToKeyword('i') == 0)
        return 0;
    a = Script_GetValue();
    if (Script_SeekToKeyword('p'))
        b = Script_GetValue();
    else
        b = 0;
    return Inventory_AddItem(a, b);
}
