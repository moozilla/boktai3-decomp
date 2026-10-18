#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void sub_0820B3E0(void);
void DragonPuppy_OnTaskDestroy(void);
s32 sub_0820B7E0(void *, void *);
void *DragonPuppy_Create(void *p)
{
    void *r = Task_Create(8, 0x198);
    if (r) {
        Task_SetCallbacks(r, sub_0820B3E0, DragonPuppy_OnTaskDestroy);
        if (sub_0820B7E0(r, p) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
