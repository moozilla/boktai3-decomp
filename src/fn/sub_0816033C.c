#include "global.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 sub_081602D0(void *, void *);
void Task_Kill(void *);
void sub_0815FF98(void);
void sub_0815FFB8(void);

void *sub_0816033C(void *p)
{
    void *r = Task_Create(0xb, 0x4C8);
    if (r != 0) {
        Task_SetCallbacks(r, sub_0815FF98, sub_0815FFB8);
        if (sub_081602D0(r, p) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
