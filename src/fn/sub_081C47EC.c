#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void *), void (*)(void *));
s32 sub_081C47A0(void *, void *);
void Task_Kill(void *);
void sub_081C46C0(void *);
void sub_081C46D0(void *);
void *sub_081C47EC(void *a)
{
    void *p = Task_Create(0xb, 0x70);
    if (p != 0) {
        Task_SetCallbacks(p, sub_081C46C0, sub_081C46D0);
        if (sub_081C47A0(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
