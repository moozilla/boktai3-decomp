#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void *), void (*)(void *));
s32 sub_081C4EF0(void *);
void Task_Kill(void *);
void sub_081C4EDC(void *);
void sub_081C4EEC(void *);
void *sub_081C5000(void)
{
    void *p = Task_Create(0xb, 0x40);
    if (p != 0) {
        Task_SetCallbacks(p, sub_081C4EDC, sub_081C4EEC);
        if (sub_081C4EF0(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
