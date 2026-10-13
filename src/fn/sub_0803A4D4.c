#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 sub_0803A434(void *);
void Task_Kill(void *);
void sub_0803A3C8(void);
void sub_0803A408(void);
void *sub_0803A4D4(void)
{
    void *p = Task_Create(11, 0xb5c);
    if (p) {
        Task_SetCallbacks(p, sub_0803A3C8, sub_0803A408);
        if (sub_0803A434(p) >= 0)
            goto ok;
        Task_Kill(p);
        return 0;
    }
ok:
    return p;
}
