#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void sub_081DEC2C(void);
void AnimPanel_RemoveAnimations(void);
s32 sub_081DEE38(void *);

void *Time_CreateDateMenu(void)
{
    void *r = Task_Create(0xb, 0x740);
    if (r != 0) {
        Task_SetCallbacks(r, sub_081DEC2C, AnimPanel_RemoveAnimations);
        if (sub_081DEE38(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
