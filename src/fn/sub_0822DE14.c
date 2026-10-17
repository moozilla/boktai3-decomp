#include "global.h"

#include "engine.h"
s32 sub_0822D6E0(s32);
/* Script: 1 when item -w is among the first 16 entries (48 with -d set),
   else 0 (tentative). */
u32 Inventory_ContainsFromScript(void)
{
    s32 id, n, i;
    if (!Script_SeekToKeyword('w'))
        return 0;
    id = Script_GetValue();
    if (Script_SeekToKeyword('d')) {
        s32 wide = Script_GetValue();
        n = 0x10;
        if (wide)
            n = 0x30;
    } else
        n = 0x10;
    for (i = 0; i < n; i++) {
        if (sub_0822D6E0(i) == id)
            return 1;
    }
    return 0;
}
