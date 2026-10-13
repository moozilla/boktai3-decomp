#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 sub_0820D5E4(void *);
void Task_Kill(void *);
void sub_0820D2C4(void);
void sub_0820D5AC(void);
void *sub_0820D614(void)
{
    void *p = Task_Create(8, 0xB1C);
    if (p) {
        Task_SetCallbacks(p, sub_0820D2C4, sub_0820D5AC);
        if (sub_0820D5E4(p) >= 0)
            goto ok;
        Task_Kill(p);
        return 0;
    }
ok:
    return p;
}
