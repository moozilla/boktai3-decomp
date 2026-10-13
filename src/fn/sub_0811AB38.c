#include "global.h"
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_0811A6C4(u8 *);
void Task_Kill(u8 *);
void sub_0811A604(void);
void sub_0811A62C(void);

u8 *sub_0811AB38(void)
{
    u8 *p;
    p = Task_Create(6, 0x968);
    if (p != 0) {
        Task_SetCallbacks(p, sub_0811A604, sub_0811A62C);
        if (sub_0811A6C4(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
