#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void *), void (*)(void *));
s32 sub_081C0B24(void *, void *);
void Task_Kill(void *);
void sub_081C0A80(void *);
void sub_081C0A9C(void *);
void *sub_081C0B94(void *a)
{
    void *p = Task_Create(8, 0x60);
    if (p != 0) {
        Task_SetCallbacks(p, sub_081C0A80, sub_081C0A9C);
        if (sub_081C0B24(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
