#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void *), void (*)(void *));
s32 sub_081C0608(void *, void *);
void Task_Kill(void *);
void sub_081C05F8(void *);
void sub_081C0604(void *);
void *sub_081C065C(void *a)
{
    void *p = Task_Create(0xb, 0x38);
    if (p != 0) {
        Task_SetCallbacks(p, sub_081C05F8, sub_081C0604);
        if (sub_081C0608(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
