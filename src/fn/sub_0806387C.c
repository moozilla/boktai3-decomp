#include "global.h"

void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 sub_0806386C(void *, void *);
void Task_Kill(void *);
void sub_0806385C(void);
void sub_08063868(void);

void *sub_0806387C(void *a)
{
    void *p = Task_Create(11, 0x50);
    if (p != 0) {
        Task_SetCallbacks(p, sub_0806385C, sub_08063868);
        if (sub_0806386C(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
