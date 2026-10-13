#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void *, void *);
s32 sub_081FEFFC(void *);
void Task_Kill(void *);
void sub_081FEE0C(void);
void sub_081FEFC4(void);

void *sub_081FF02C(void)
{
    void *p = Task_Create(8, 0x171c);
    if (p) {
        Task_SetCallbacks(p, sub_081FEE0C, sub_081FEFC4);
        if (sub_081FEFFC(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
