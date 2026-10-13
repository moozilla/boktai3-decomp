#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void sub_0803F3A4(void);
void sub_0803F3D8(void);
s32 sub_0803F40C(void *);
void *sub_0803F458(void)
{
    void *r = Task_Create(10, 0x9a0);
    if (r) {
        Task_SetCallbacks(r, sub_0803F3A4, sub_0803F3D8);
        if (sub_0803F40C(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
