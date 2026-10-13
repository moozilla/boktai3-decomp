#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void sub_081E132C(void);
void sub_081E154C(void);
s32 sub_081E15C8(void *);

void *sub_081E1AB4(void)
{
    void *r = Task_Create(0xb, 0x4f0);
    if (r != 0) {
        Task_SetCallbacks(r, sub_081E132C, sub_081E154C);
        if (sub_081E15C8(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
