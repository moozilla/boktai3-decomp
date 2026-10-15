#include "breakable_prop.h"
#define BreakableProp_CreateFromScript sub_080519F8
struct BreakableProp *BreakableProp_CreateFromScript(u32 a, u32 b)
{
    struct BreakableProp *p = Task_Create(8, 0xD0);
    if (p) {
        Task_SetCallbacks(p, (void (*)(void *))sub_080516FC, (void (*)(void *))sub_0805173C);
        if (sub_08051858(p, a, b) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
