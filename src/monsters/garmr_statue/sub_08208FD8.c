#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void CannonTurret_UpdateCombat(void);
void CannonTurret_OnTaskDestroy(void);
s32 CannonTurret_InitializeFromScript(void *, void *);
void *CannonTurret_Create(void *p)
{
    void *r = Task_Create(8, 0x168);
    if (r) {
        Task_SetCallbacks(r, CannonTurret_UpdateCombat, CannonTurret_OnTaskDestroy);
        if (CannonTurret_InitializeFromScript(r, p) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
