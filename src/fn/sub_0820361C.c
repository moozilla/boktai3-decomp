#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void sub_08203204(void);
void sub_08203370(void);
s32 sub_0820337C(void *, void *);
void *sub_0820361C(void *p)
{
    void *r = Task_Create(8, 0x104);
    if (r) {
        Task_SetCallbacks(r, sub_08203204, sub_08203370);
        if (sub_0820337C(r, p) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
