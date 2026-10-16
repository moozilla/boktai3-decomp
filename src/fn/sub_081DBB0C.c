#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void sub_081DB474(void);
void sub_081DB75C(void);
s32 sub_081DB794(void *);

void *Options_Create(void)
{
    void *r = Task_Create(0xb, 0x304);
    if (r != 0) {
        Task_SetCallbacks(r, sub_081DB474, sub_081DB75C);
        if (sub_081DB794(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
