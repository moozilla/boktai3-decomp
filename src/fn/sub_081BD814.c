#include "global.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 sub_081BD708(void *);
void Task_Kill(void *);
void DarkLoan_OnUpdate(void);
void DarkLoan_OnDestroy(void);

void *DarkLoan_Create(void)
{
    void *r = Task_Create(0xb, 2356);
    if (r != 0) {
        Task_SetCallbacks(r, DarkLoan_OnUpdate, DarkLoan_OnDestroy);
        if (sub_081BD708(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
