#include "global.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 sub_08163700(void *);
void Task_Kill(void *);
void sub_081636A0(void);
void sub_081636C0(void);
extern void *gUnk_02000210;

void *sub_081637D0(void)
{
    void *r = gUnk_02000210;
    void *q;
    if (r == 0) {
        q = Task_Create(0xb, 0x354);
        if (q != 0) {
            Task_SetCallbacks(q, sub_081636A0, sub_081636C0);
            if (sub_08163700(q) < 0) {
                Task_Kill(q);
                return 0;
            }
        }
        r = q;
    }
    return r;
}
