#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void sub_08201D38(void);
void sub_08202120(void);
s32 sub_0820212C(void *, void *);
void *sub_08202608(void *p)
{
    void *r = Task_Create(8, 0x1e0);
    if (r) {
        Task_SetCallbacks(r, sub_08201D38, sub_08202120);
        if (sub_0820212C(r, p) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
