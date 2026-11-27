#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void HomingProjectile_UpdateManager(void);
void HomingProjectile_DestroyManager(void);
s32 HomingProjectile_InitManager(void *);
void *HomingProjectile_CreateManager(void)
{
    void *r = Task_Create(8, 0x480);
    if (r) {
        Task_SetCallbacks(r, HomingProjectile_UpdateManager, HomingProjectile_DestroyManager);
        if (HomingProjectile_InitManager(r) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
