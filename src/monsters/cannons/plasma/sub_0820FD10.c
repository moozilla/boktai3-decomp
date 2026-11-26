#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void PlasmaCannon_UpdateDamageState(void);
void PlasmaCannon_OnTaskDestroy(void);
s32 PlasmaCannon_InitializeFromScript(void *, void *);
void *PlasmaCannon_Create(void *p)
{
    void *r = Task_Create(8, 0x198);
    if (r) {
        Task_SetCallbacks(r, PlasmaCannon_UpdateDamageState, PlasmaCannon_OnTaskDestroy);
        if (PlasmaCannon_InitializeFromScript(r, p) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
