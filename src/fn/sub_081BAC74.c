#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 sub_081BABB0(void *, void *);
void Task_Kill(void *);
void sub_081BAB20(void);
void sub_081BAB48(void);
void *sub_081BAC74(void *a)
{
    void *p = Task_Create(8, 0x268);
    if (p) {
        Task_SetCallbacks(p, sub_081BAB20, sub_081BAB48);
        if (sub_081BABB0(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
