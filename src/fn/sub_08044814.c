#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 sub_080447D4(void *);
void Task_Kill(void *);
void sub_08044594(void);
void sub_080447AC(void);
void *sub_08044814(void)
{
    void *p = Task_Create(8, 0x1590);
    if (p) {
        Task_SetCallbacks(p, sub_08044594, sub_080447AC);
        if (sub_080447D4(p) >= 0)
            goto ok;
        Task_Kill(p);
        return 0;
    }
ok:
    return p;
}
