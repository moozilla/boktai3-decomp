#include "global.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 sub_081B74A8(void *);
void Task_Kill(void *);
void sub_081B744C(void);
void sub_081B748C(void);

void *sub_081B74D0(void)
{
    void *r = Task_Create(9, 0x27c);
    if (r != 0) {
        Task_SetCallbacks(r, sub_081B744C, sub_081B748C);
        if (sub_081B74A8(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
