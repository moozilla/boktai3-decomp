#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void sub_08210398(void);
void PinkSpine_OnTaskDestroy(void);
s32 sub_08210654(void *, void *);
void *PinkSpine_Create(void *p)
{
    void *r = Task_Create(8, 0x160);
    if (r) {
        Task_SetCallbacks(r, sub_08210398, PinkSpine_OnTaskDestroy);
        if (sub_08210654(r, p) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
