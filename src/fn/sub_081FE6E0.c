#include "global.h"
extern u8 *gUnk_020005EC;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_081FE68C(u8 *);
void Task_Kill(u8 *);
void sub_081FE2C4(void);
void sub_081FE644(void);
u8 *sub_081FE6E0(void)
{
    u8 *r;
    if (gUnk_020005EC != 0)
        return gUnk_020005EC;
    r = Task_Create(10, 0xef8);
    if (r) {
        Task_SetCallbacks(r, sub_081FE2C4, sub_081FE644);
        if (sub_081FE68C(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
