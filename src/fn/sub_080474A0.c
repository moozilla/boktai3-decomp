#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void sub_08047338(void);
void sub_080473F0(void);
s32 sub_08047424(void *, void *);
void *sub_080474A0(void *p)
{
    void *r = Task_Create(10, 0x598);
    if (r) {
        Task_SetCallbacks(r, sub_08047338, sub_080473F0);
        if (sub_08047424(r, p) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
