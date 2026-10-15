#include "breakable_prop.h"
#define BreakableProp_Tick sub_080516FC
u32 BreakableProp_Tick(struct BreakableProp *prop)
{
    if (prop->destroyRequested != 0) {
        u32 *scriptId;
        u32 script;
        prop->sprite.flags |= 1;
        sub_080515DC(prop);
        scriptId = &prop->scriptId;
        script = *scriptId;
        if (script != 0) {
            *scriptId = 0;
            Script_Call(script, 0);
        }
        Task_Kill(prop);
    }
    return 0;
}
