#include "global.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 sub_0815F2C8(void *, void *, void *);
void Task_Kill(void *);
void sub_0815F0D0(void);
void sub_0815F224(void);

void *sub_0815F4F4(void *a, void *b)
{
    void *r = Task_Create(6, 0xCAC);
    if (r != 0) {
        Task_SetCallbacks(r, sub_0815F0D0, sub_0815F224);
        if (sub_0815F2C8(r, a, b) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
