#include "global.h"
void *sub_0802140C(u32);
void *Task_Create(u32, u32);
void sub_081FAD14(void);
void sub_081FAD74(void);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 sub_081FAE0C(void *, void *);
void Task_Kill(void *);
void *Ratatosk_Create(void *arg)
{
    void *p = sub_0802140C(4);
    if (p)
        return p;
    p = Task_Create(8, 0xf30);
    if (p) {
        Task_SetCallbacks(p, sub_081FAD14, sub_081FAD74);
        if (sub_081FAE0C(p, arg) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
