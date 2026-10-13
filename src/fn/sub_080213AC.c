#include "global.h"

void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 sub_08021394(void *, void *);
void Task_Kill(void *);
void sub_08021360(void);
void sub_08021384(void);

void *sub_080213AC(void *a)
{
    void *p = Task_Create(3, 0x5C);
    if (p != 0) {
        Task_SetCallbacks(p, sub_08021360, sub_08021384);
        if (sub_08021394(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
