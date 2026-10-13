#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void sub_0820F378(void);
void sub_0820F824(void);
s32 sub_0820F830(void *, void *);
void *sub_0820FD10(void *p)
{
    void *r = Task_Create(8, 0x198);
    if (r) {
        Task_SetCallbacks(r, sub_0820F378, sub_0820F824);
        if (sub_0820F830(r, p) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
