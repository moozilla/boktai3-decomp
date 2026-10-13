#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void sub_081E09A8(void);
void sub_081E0B28(void);
s32 sub_081E0B4C(void *);

void *sub_081E0D18(void)
{
    void *r = Task_Create(0xb, 0x1f8);
    if (r != 0) {
        Task_SetCallbacks(r, sub_081E09A8, sub_081E0B28);
        if (sub_081E0B4C(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
