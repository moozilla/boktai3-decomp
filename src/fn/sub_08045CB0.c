#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void sub_08045C20(void);
void sub_08045C38(void);
s32 sub_08045C44(void *, void *);
void *sub_08045CB0(void *p)
{
    void *r = Task_Create(10, 0x238);
    if (r) {
        Task_SetCallbacks(r, sub_08045C20, sub_08045C38);
        if (sub_08045C44(r, p) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
