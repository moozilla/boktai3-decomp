#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void sub_08206030(void);
void sub_0820608C(void);
s32 sub_08206098(void *, void *);
void *Ooze_Create(void *p)
{
    void *r = Task_Create(8, 0x160);
    if (r) {
        Task_SetCallbacks(r, sub_08206030, sub_0820608C);
        if (sub_08206098(r, p) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
