#include "breakable_prop.h"
struct BreakableProp *BreakableProp_CreateFromScript(u32 a, u32 b)
{
    struct BreakableProp *p = Task_Create(8, 0xD0);
    if (p) {
        Task_SetCallbacks(p, (void (*)(void *))BreakableProp_Tick,
                          (void (*)(void *))BreakableProp_Destroy);
        if (BreakableProp_InitializeFromScript(p, a, b) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
