#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void *), void (*)(void *));
s32 sub_081C0390(void *);
void Task_Kill(void *);
void sub_081C0344(void *);
void sub_081C0378(void *);
void *sub_081C0434(void)
{
    void *p = Task_Create(9, 0xac);
    if (p != 0) {
        Task_SetCallbacks(p, sub_081C0344, sub_081C0378);
        if (sub_081C0390(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
