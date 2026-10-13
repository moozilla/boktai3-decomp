#include "global.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 sub_081E013C(void *);
void Task_Kill(void *);
void sub_081DFCB4(void);
void sub_081E00E8(void);

void *sub_081E0474(void)
{
    void *r = Task_Create(0xb, 2076);
    if (r != 0) {
        Task_SetCallbacks(r, sub_081DFCB4, sub_081E00E8);
        if (sub_081E013C(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
