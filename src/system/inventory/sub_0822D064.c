#include "global.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
s32 Inventory_HasItem(s32);
s32 Inventory_HasItemFromScript(void)
{
    if (Script_SeekToKeyword(0x69))
        return Inventory_HasItem(Script_GetValue());
    return 0;
}
