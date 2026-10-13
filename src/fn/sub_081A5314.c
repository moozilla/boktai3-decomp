#include "global.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 sub_081A522C(void *, s32, s32, s32);
void Task_Kill(void *);
void sub_081A516C(void);
void sub_081A5198(void);

void *sub_081A5314(s32 a, s32 b, s32 c)
{
    void *r = Task_Create(10, 0x13c);
    if (r != 0) {
        Task_SetCallbacks(r, sub_081A516C, sub_081A5198);
        if (sub_081A522C(r, a, b, c) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
