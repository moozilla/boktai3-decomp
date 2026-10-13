#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void sub_081FBF2C(void);
void sub_081FC16C(void);
s32 sub_081FC1A4(void *);
void *sub_081FC1FC(void)
{
    void *r = Task_Create(8, 0xf40);
    if (r) {
        Task_SetCallbacks(r, sub_081FBF2C, sub_081FC16C);
        if (sub_081FC1A4(r) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
