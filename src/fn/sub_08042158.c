#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 sub_08042068(void *);
void Task_Kill(void *);
void sub_08041F3C(void);
void sub_08042044(void);
void *sub_08042158(void)
{
    void *p = Task_Create(4, 0x1088);
    if (p) {
        Task_SetCallbacks(p, sub_08041F3C, sub_08042044);
        if (sub_08042068(p) >= 0)
            goto ok;
        Task_Kill(p);
        return 0;
    }
ok:
    return p;
}
