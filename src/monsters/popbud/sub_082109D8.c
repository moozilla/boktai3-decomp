#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void PinkSpine_UpdateDamageAnimation(void);
void PinkSpine_OnTaskDestroy(void);
s32 PinkSpine_InitFromScript(void *, void *);
void *PinkSpine_Create(void *p)
{
    void *r = Task_Create(8, 0x160);
    if (r) {
        Task_SetCallbacks(r, PinkSpine_UpdateDamageAnimation, PinkSpine_OnTaskDestroy);
        if (PinkSpine_InitFromScript(r, p) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
