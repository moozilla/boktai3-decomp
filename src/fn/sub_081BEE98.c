#include "global.h"
s32 SceneInput_TickPairTransition(u8 *);
void sub_081BECF0(u8 *);
void SceneInventory_BeginCategoryBrowse(u8 *);
void Storage_UpdateItemTransition(u8 *p)
{
    if (SceneInput_TickPairTransition(p + 0x1558)) {
        sub_081BECF0(p);
        SceneInventory_BeginCategoryBrowse(p);
    }
}
