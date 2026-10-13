#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 sub_081B866C(void *, void *);
void Task_Kill(void *);
void sub_081B8518(void);
void sub_081B854C(void);
void *sub_081B897C(void *a)
{
    void *p = Task_Create(8, 0x140);
    if (p) {
        Task_SetCallbacks(p, sub_081B8518, sub_081B854C);
        if (sub_081B866C(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
