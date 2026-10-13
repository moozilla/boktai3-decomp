#include "global.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 sub_081604F8(void *, void *, void *, void *);
void Task_Kill(void *);
void sub_081604C0(void);
void sub_081604EC(void);

void *sub_081605DC(void *a, void *b, void *c)
{
    void *r = Task_Create(5, 0x78);
    if (r != 0) {
        Task_SetCallbacks(r, sub_081604C0, sub_081604EC);
        if (sub_081604F8(r, a, b, c) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
