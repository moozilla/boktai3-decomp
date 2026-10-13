#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 sub_080456C8(void *);
void Task_Kill(void *);
void sub_08045574(void);
void sub_08045688(void);
void *sub_08045704(void)
{
    void *p = Task_Create(8, 0x13dc);
    if (p) {
        Task_SetCallbacks(p, sub_08045574, sub_08045688);
        if (sub_080456C8(p) >= 0)
            goto ok;
        Task_Kill(p);
        return 0;
    }
ok:
    return p;
}
