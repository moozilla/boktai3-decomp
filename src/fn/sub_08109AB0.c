#include "global.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_08109A04(u8 *);
void Task_Kill(u8 *);
void sub_081099A4(void);
void sub_081099E4(void);

u8 *sub_08109AB0(void)
{
    u8 *p;
    p = Task_Create(11, 0x4C);
    if (p != 0) {
        Task_SetCallbacks(p, sub_081099A4, sub_081099E4);
        if (sub_08109A04(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
