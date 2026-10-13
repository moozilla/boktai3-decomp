#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void *), void (*)(void *));
s32 sub_081C6570(void *);
void Task_Kill(void *);
void sub_081C61CC(void *);
void sub_081C61F4(void *);
void *sub_081C6614(void)
{
    void *p = Task_Create(0xb, 0x378);
    if (p != 0) {
        Task_SetCallbacks(p, sub_081C61CC, sub_081C61F4);
        if (sub_081C6570(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
