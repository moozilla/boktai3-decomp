#include "global.h"

u32 Task_Create(u32, u32);
void Task_SetCallbacks(u32, void (*)(void), void (*)(void));
s32 sub_081D3094(u32);
void Task_Kill(u32);
void sub_081D305C(void);
void sub_081D3078(void);

u32 sub_081D313C(void)
{
    u32 r = Task_Create(8, 0x3a8);
    if (r != 0) {
        Task_SetCallbacks(r, sub_081D305C, sub_081D3078);
        if (sub_081D3094(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
