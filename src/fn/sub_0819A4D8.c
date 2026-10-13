#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 sub_0819A470(void *);
void Task_Kill(void *);
void sub_0819A23C(void);
void sub_0819A414(void);
void *sub_0819A4D8(u16 a)
{
    u32 *p = Task_Create(8, 0x1328);
    if (p) {
        p[6] = a;
        Task_SetCallbacks(p, sub_0819A23C, sub_0819A414);
        if (sub_0819A470(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
