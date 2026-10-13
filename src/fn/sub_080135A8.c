#include "global.h"

void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 sub_08013584(void *, void *);
void Task_Kill(void *);
void sub_08013558(void);
void sub_08013578(void);

void *sub_080135A8(void *a)
{
    void *p = Task_Create(9, 0x20);
    if (p != 0) {
        Task_SetCallbacks(p, sub_08013558, sub_08013578);
        if (sub_08013584(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
