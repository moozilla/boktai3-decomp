#include "global.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_081D750C(u8 *);
void Task_Kill(u8 *);
void sub_081D7434(void);
void sub_081D7508(void);

u8 *sub_081D75AC(void)
{
    u8 *p;
    p = Task_Create(11, 0x20);
    if (p != 0) {
        Task_SetCallbacks(p, sub_081D7434, sub_081D7508);
        if (sub_081D750C(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
