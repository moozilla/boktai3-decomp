#include "engine/engine.h"
#include "global.h"
s32 Inventory_HasItem(s32);
s32 Inventory_HasItemFromScript(void)
{
    if (Script_SeekToKeyword('i'))
        return Inventory_HasItem(Script_GetValue());
    return 0;
}
