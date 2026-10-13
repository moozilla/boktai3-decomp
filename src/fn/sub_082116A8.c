#include "global.h"

void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 sub_0821159C(void *, void *);
void Task_Kill(void *);
void sub_082112F4(void);
void sub_08211588(void);

void *sub_082116A8(void *a)
{
    void *p = Task_Create(8, 0xba8);
    if (p != 0) {
        Task_SetCallbacks(p, sub_082112F4, sub_08211588);
        if (sub_0821159C(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
