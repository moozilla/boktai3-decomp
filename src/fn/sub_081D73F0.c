#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 sub_081D7300(void *, void *);
void Task_Kill(void *);
void sub_081D72A4(void);
void sub_081D72FC(void);
void *sub_081D73F0(void *a)
{
    void *p = Task_Create(8, 0x328);
    if (p) {
        Task_SetCallbacks(p, sub_081D72A4, sub_081D72FC);
        if (sub_081D7300(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
