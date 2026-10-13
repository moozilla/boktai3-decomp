#include "global.h"

void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 sub_0803F6C4(void *, void *);
void Task_Kill(void *);
void sub_0803F690(void);
void sub_0803F6B0(void);

void *sub_0803F6F0(void *a)
{
    void *p = Task_Create(9, 0x20);
    if (p != 0) {
        Task_SetCallbacks(p, sub_0803F690, sub_0803F6B0);
        if (sub_0803F6C4(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
