#include "global.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 sub_081F2C64(void *, void *, void *, void *);
void Task_Kill(void *);
void sub_081F2BB0(void);
void sub_081F2BFC(void);

void *sub_081F2C8C(void *a, void *b, void *c)
{
    void *r = Task_Create(10, 0x68);
    if (r != 0) {
        Task_SetCallbacks(r, sub_081F2BB0, sub_081F2BFC);
        if (sub_081F2C64(r, a, b, c) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
