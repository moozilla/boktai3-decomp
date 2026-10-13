#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 sub_0803D514(void *);
void Task_Kill(void *);
void sub_0803D430(void);
void sub_0803D4D0(void);
void *sub_0803D5A8(void)
{
    void *p = Task_Create(8, 0x2360);
    if (p) {
        Task_SetCallbacks(p, sub_0803D430, sub_0803D4D0);
        if (sub_0803D514(p) >= 0)
            goto ok;
        Task_Kill(p);
        return 0;
    }
ok:
    return p;
}
