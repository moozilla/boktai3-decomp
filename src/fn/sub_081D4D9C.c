#include "global.h"

u32 Task_Create(u32, u32);
void Task_SetCallbacks(u32, void (*)(void), void (*)(void));
s32 sub_081D4CE0(u32, u32);
void Task_Kill(u32);
void sub_081D4AC0(void);
void sub_081D4CA4(void);

u32 sub_081D4D9C(u32 a)
{
    u32 r = Task_Create(8, 0xa10);
    if (r != 0) {
        Task_SetCallbacks(r, sub_081D4AC0, sub_081D4CA4);
        if (sub_081D4CE0(r, a) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
