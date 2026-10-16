#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void sub_0820C9AC(void);
void sub_0820CCF4(void);
s32 sub_0820CD00(void *, void *);
void *Screamer_Create(void *p)
{
    void *r = Task_Create(8, 0x220);
    if (r) {
        Task_SetCallbacks(r, sub_0820C9AC, sub_0820CCF4);
        if (sub_0820CD00(r, p) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
