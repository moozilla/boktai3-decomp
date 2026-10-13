#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void sub_081FFAB8(void);
void sub_081FFD78(void);
s32 sub_081FFDBC(void *);
void *sub_081FFE20(void)
{
    void *r = Task_Create(8, 0x480);
    if (r) {
        Task_SetCallbacks(r, sub_081FFAB8, sub_081FFD78);
        if (sub_081FFDBC(r) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
