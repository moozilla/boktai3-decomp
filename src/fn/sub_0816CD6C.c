#include "global.h"
#include "ram.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void Inventory_SetUsePermissionFlagsFromScript(void)
{
    s32 i;
    gInventoryUseFlags = 0;
    if (Script_SeekToKeyword(0x66)) {
        for (i = 0; i <= 3; i++) {
            if (Script_GetValue())
                gInventoryUseFlags |= 1 << i;
        }
    }
}
