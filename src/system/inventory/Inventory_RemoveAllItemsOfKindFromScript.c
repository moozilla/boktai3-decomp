#include "engine/engine.h"
#include "global.h"
void Inventory_RemoveItem(u32);
u32 Inventory_RemoveAllItemsOfKindFromScript(void)
{
    u32 a;
    s32 i;
    if (Script_SeekToKeyword('i') == 0)
        return 0;
    a = Script_GetValue();
    for (i = 0; i < 16; i++)
        Inventory_RemoveItem(a);
}
