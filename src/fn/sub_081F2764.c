#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void sub_081F26BC(void);
void sub_081F2700(void);
s32 sub_081F273C(void *);

void *sub_081F2764(void)
{
    void *r = Task_Create(8, 0x980);
    if (r != 0) {
        Task_SetCallbacks(r, sub_081F26BC, sub_081F2700);
        if (sub_081F273C(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
