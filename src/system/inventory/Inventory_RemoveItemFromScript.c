#include "engine/engine.h"
#include "global.h"
s32 Inventory_RemoveItem(s32);
s32 Inventory_RemoveItemFromScript(void)
{
    if (Script_SeekToKeyword('i'))
        return Inventory_RemoveItem(Script_GetValue());
    return 0;
}
