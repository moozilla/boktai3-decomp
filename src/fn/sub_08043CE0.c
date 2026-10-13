#include "global.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 sub_08043C4C(void *, s32, s32, s32);
void Task_Kill(void *);
void sub_08043ACC(void);
void sub_08043C24(void);

void *sub_08043CE0(s32 a, s32 b, s32 c)
{
    void *r = Task_Create(8, 0x1720);
    if (r != 0) {
        Task_SetCallbacks(r, sub_08043ACC, sub_08043C24);
        if (sub_08043C4C(r, a, b, c) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
