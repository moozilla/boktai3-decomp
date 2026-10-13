#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void sub_081DBB4C(void);
void sub_081DBDC8(void);
s32 sub_081DBE04(void *);

void *sub_081DBFEC(void)
{
    void *r = Task_Create(0xb, 0x1dc);
    if (r != 0) {
        Task_SetCallbacks(r, sub_081DBB4C, sub_081DBDC8);
        if (sub_081DBE04(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
