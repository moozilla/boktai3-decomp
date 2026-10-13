#include "global.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_0806F870(u8 *);
void Task_Kill(u8 *);
void sub_0806F838(void);
void sub_0806F86C(void);

u8 *sub_0806F954(void)
{
    u8 *p;
    p = Task_Create(8, 0x2c);
    if (p != 0) {
        Task_SetCallbacks(p, sub_0806F838, sub_0806F86C);
        if (sub_0806F870(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
