#include "global.h"

u32 Task_Create(u32, u32);
void Task_SetCallbacks(u32, void (*)(void), void (*)(void));
s32 sub_081D15EC(u32, u32);
void Task_Kill(u32);
void sub_081D1404(void);
void sub_081D15C8(void);

u32 sub_081D1688(u32 a)
{
    u32 r = Task_Create(0xb, 0x1fc);
    if (r != 0) {
        Task_SetCallbacks(r, sub_081D1404, sub_081D15C8);
        if (sub_081D15EC(r, a) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
