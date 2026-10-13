#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void *), void (*)(void *));
s32 sub_081C5D40(void *);
void Task_Kill(void *);
void sub_081C59A4(void *);
void sub_081C59C4(void *);
void *sub_081C5E44(void)
{
    void *p = Task_Create(8, 0x314);
    if (p != 0) {
        Task_SetCallbacks(p, sub_081C59A4, sub_081C59C4);
        if (sub_081C5D40(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
