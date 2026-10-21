#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void DragonPuppy_UpdateCombat(void);
void DragonPuppy_OnTaskDestroy(void);
s32 DragonPuppy_InitializeFromScript(void *, void *);
void *DragonPuppy_Create(void *p)
{
    void *r = Task_Create(8, 0x198);
    if (r) {
        Task_SetCallbacks(r, DragonPuppy_UpdateCombat, DragonPuppy_OnTaskDestroy);
        if (DragonPuppy_InitializeFromScript(r, p) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
