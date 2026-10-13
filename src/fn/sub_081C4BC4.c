#include "global.h"
extern void *gUnk_0200026C;
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void *), void (*)(void *));
s32 sub_081C49F4(void *, void *);
void Task_Kill(void *);
void sub_081C49C0(void *);
void sub_081C49DC(void *);
void *sub_081C4BC4(void *a)
{
    void *p;
    if (gUnk_0200026C != 0)
        return gUnk_0200026C;
    p = Task_Create(0xb, 0xe4);
    if (p != 0) {
        Task_SetCallbacks(p, sub_081C49C0, sub_081C49DC);
        if (sub_081C49F4(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    gUnk_0200026C = p;
    return p;
}
