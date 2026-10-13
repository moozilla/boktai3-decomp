#include "global.h"

void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 sub_080138E0(void *, void *);
void Task_Kill(void *);
void sub_080138B4(void);
void sub_080138D4(void);

void *sub_080138F0(void *a)
{
    void *p = Task_Create(9, 0x20);
    if (p != 0) {
        Task_SetCallbacks(p, sub_080138B4, sub_080138D4);
        if (sub_080138E0(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
