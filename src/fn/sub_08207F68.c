#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void VulcanCannon_UpdateDamageState(void);
void VulcanCannon_Destroy(void);
s32 VulcanCannon_InitializeFromScript(void *, void *);
void *VulcanCannon_Create(void *p)
{
    void *r = Task_Create(8, 0x1a8);
    if (r) {
        Task_SetCallbacks(r, VulcanCannon_UpdateDamageState, VulcanCannon_Destroy);
        if (VulcanCannon_InitializeFromScript(r, p) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
