#include "global.h"

extern void *gUnk_0200059C;
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 sub_081625C0(void *, void *, void *, void *);
void Task_Kill(void *);
void sub_0816233C(void);
void sub_08162598(void);

void *sub_081626E0(void *a, void *b, void *c)
{
    void *r;
    if (gUnk_0200059C != 0)
        return gUnk_0200059C;
    r = Task_Create(0xa, 0x5A8);
    if (r != 0) {
        Task_SetCallbacks(r, sub_0816233C, sub_08162598);
        if (sub_081625C0(r, a, b, c) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    gUnk_0200059C = r;
    return r;
}
