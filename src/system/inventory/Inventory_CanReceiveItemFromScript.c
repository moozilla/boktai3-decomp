#include "engine/engine.h"
#include "global.h"
u32 sub_0822D490(u32, u32);
u32 Inventory_CanReceiveItemFromScript(void)
{
    u32 a, r;
    if (Script_SeekToKeyword('k') == 0)
        r = 0;
    else {
        a = Script_GetValue();
        if (Script_SeekToKeyword('i') == 0)
            r = 0;
        else
            r = sub_0822D490(a, Script_GetValue());
    }
    return r;
}
