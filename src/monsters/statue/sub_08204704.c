#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void Statue_UpdateDamageState(void);
void Statue_Destroy(void);
s32 Statue_InitializeFromScript(void *, void *);
void *Statue_Create(void *p)
{
    void *r = Task_Create(8, 0x198);
    if (r) {
        Task_SetCallbacks(r, Statue_UpdateDamageState, Statue_Destroy);
        if (Statue_InitializeFromScript(r, p) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
