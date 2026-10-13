#include "global.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 sub_081E056C(void *);
void Task_Kill(void *);
void sub_081E04B4(void);
void sub_081E0560(void);

void *sub_081E07D8(void)
{
    void *r = Task_Create(0xb, 1228);
    if (r != 0) {
        Task_SetCallbacks(r, sub_081E04B4, sub_081E0560);
        if (sub_081E056C(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
