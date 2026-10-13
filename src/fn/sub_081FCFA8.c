#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void sub_081FCA44(void);
void sub_081FCF38(void);
s32 sub_081FCF74(void *);
void *sub_081FCFA8(void)
{
    void *r = Task_Create(8, 0x11a0);
    if (r) {
        Task_SetCallbacks(r, sub_081FCA44, sub_081FCF38);
        if (sub_081FCF74(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
