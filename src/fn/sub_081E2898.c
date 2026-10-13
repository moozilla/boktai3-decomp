#include "global.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 sub_081E27A8(void *);
void Task_Kill(void *);
void TimedPresentation_Update(void);
void sub_081E2794(void);

void *sub_081E2898(void)
{
    void *r = Task_Create(0xb, 0x38);
    if (r != 0) {
        Task_SetCallbacks(r, TimedPresentation_Update, sub_081E2794);
        if (sub_081E27A8(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
