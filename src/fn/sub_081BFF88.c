#include "global.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 sub_081BFF18(void *);
void Task_Kill(void *);
void sub_081BFEAC(void);
void sub_081BFEDC(void);

void *sub_081BFF88(void)
{
    void *r = Task_Create(0xb, 6136);
    if (r != 0) {
        Task_SetCallbacks(r, sub_081BFEAC, sub_081BFEDC);
        if (sub_081BFF18(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
