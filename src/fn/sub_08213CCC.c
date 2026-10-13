#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
void Task_Kill(void *);
s32 sub_08213B64(void *, u32);
void sub_08213888(void);
void sub_08213B50(void);
void *sub_08213CCC(u32 a)
{
    void *p = Task_Create(8, 0x724);
    if (p) {
        Task_SetCallbacks(p, sub_08213888, sub_08213B50);
        if (sub_08213B64(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
