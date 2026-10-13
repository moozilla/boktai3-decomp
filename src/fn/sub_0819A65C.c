#include "global.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 sub_0819A5AC(void *);
void Task_Kill(void *);
void sub_0819A564(void);
void sub_0819A59C(void);

void *sub_0819A65C(void)
{
    void *r = Task_Create(8, 0x7c);
    if (r != 0) {
        Task_SetCallbacks(r, sub_0819A564, sub_0819A59C);
        if (sub_0819A5AC(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
