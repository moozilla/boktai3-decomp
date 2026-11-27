#include "global.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
s32 Inventory_RemoveItem(s32);
s32 Inventory_RemoveItemFromScript(void)
{
    if (Script_SeekToKeyword(0x69))
        return Inventory_RemoveItem(Script_GetValue());
    return 0;
}
