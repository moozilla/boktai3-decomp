#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void sub_08208740(void);
void sub_08208BD8(void);
s32 sub_08208BE4(void *, void *);
void *sub_08208FD8(void *p)
{
    void *r = Task_Create(8, 0x168);
    if (r) {
        Task_SetCallbacks(r, sub_08208740, sub_08208BD8);
        if (sub_08208BE4(r, p) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
