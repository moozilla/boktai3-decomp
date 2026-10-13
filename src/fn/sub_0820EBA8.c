#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void sub_0820E2A4(void);
void sub_0820E718(void);
s32 sub_0820E724(void *, void *);
void *sub_0820EBA8(void *p)
{
    void *r = Task_Create(8, 0x17c);
    if (r) {
        Task_SetCallbacks(r, sub_0820E2A4, sub_0820E718);
        if (sub_0820E724(r, p) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
