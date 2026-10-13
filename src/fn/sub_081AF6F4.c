#include "global.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 sub_081AF640(void *, s32, s32);
void Task_Kill(void *);
void sub_081AF5EC(void);
void sub_081AF628(void);

void *sub_081AF6F4(s32 a, s32 b)
{
    void *r = Task_Create(9, 0x64);
    if (r != 0) {
        Task_SetCallbacks(r, sub_081AF5EC, sub_081AF628);
        if (sub_081AF640(r, a, b) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
