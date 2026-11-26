#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void Popbud_UpdateDamageAnimation(void);
void Popbud_OnTaskDestroy(void);
s32 Popbud_InitFromScript(void *, void *);
void *Popbud_Create(void *p)
{
    void *r = Task_Create(8, 0x160);
    if (r) {
        Task_SetCallbacks(r, Popbud_UpdateDamageAnimation, Popbud_OnTaskDestroy);
        if (Popbud_InitFromScript(r, p) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
