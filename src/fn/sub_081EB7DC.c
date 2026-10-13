#include "global.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 sub_081EB5A8(void *);
void Task_Kill(void *);
void sub_081EAB64(void);
void sub_081EB598(void);

void *sub_081EB7DC(void)
{
    void *r = Task_Create(0xb, 1900);
    if (r != 0) {
        Task_SetCallbacks(r, sub_081EAB64, sub_081EB598);
        if (sub_081EB5A8(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
