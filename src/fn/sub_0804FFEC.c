#include "global.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
void Task_Kill(void *);
void sub_0804FF48(void);
void sub_0804FF70(void);
s32 sub_0804FFDC(void *);

void *sub_0804FFEC(void)
{
    void *p = Task_Create(9, 0x7FC);
    if (p) {
        Task_SetCallbacks(p, sub_0804FF48, sub_0804FF70);
        if (sub_0804FFDC(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
