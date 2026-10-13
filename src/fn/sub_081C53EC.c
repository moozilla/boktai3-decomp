#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void *), void (*)(void *));
s32 sub_081C5308(void *, void *);
void Task_Kill(void *);
void sub_081C52DC(void *);
void sub_081C52FC(void *);
void *sub_081C53EC(void *a)
{
    void *p = Task_Create(8, 0xdc);
    if (p != 0) {
        Task_SetCallbacks(p, sub_081C52DC, sub_081C52FC);
        if (sub_081C5308(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
