#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 sub_081BC22C(void *);
void Task_Kill(void *);
void sub_081BBD94(void);
void sub_081BBDA8(void);
void *sub_081BC27C(void)
{
    void *p = Task_Create(0xb, 0x734);
    if (p) {
        Task_SetCallbacks(p, sub_081BBD94, sub_081BBDA8);
        if (sub_081BC22C(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
