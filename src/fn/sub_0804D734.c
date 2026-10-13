#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void *, void *);
s32 sub_0804D6DC(void *, u32, u32);
void Task_Kill(void *);
void sub_0804D620(void);
void sub_0804D6D8(void);

void *sub_0804D734(u32 a, u32 b)
{
    void *p = Task_Create(8, 0x1C);
    if (p) {
        Task_SetCallbacks(p, sub_0804D620, sub_0804D6D8);
        if (sub_0804D6DC(p, a, b) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
