#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 sub_08202A88(void *);
void Task_Kill(void *);
void sub_08202730(void);
void sub_08202A4C(void);
void *sub_08202ABC(void)
{
    void *p = Task_Create(8, 0x129C);
    if (p) {
        Task_SetCallbacks(p, sub_08202730, sub_08202A4C);
        if (sub_08202A88(p) >= 0)
            goto ok;
        Task_Kill(p);
        return 0;
    }
ok:
    return p;
}
