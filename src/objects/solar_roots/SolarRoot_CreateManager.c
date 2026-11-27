#include "global.h"
#include "engine/ram.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 SolarRoot_InitManager(void *);
void Task_Kill(void *);
void SolarRoot_UpdateManager(void);
void GrowthManager_Destroy(void);

void *SolarRoot_CreateManager(void)
{
    void *r = Task_Create(8, 0x4c0);
    if (r != 0) {
        Task_SetCallbacks(r, SolarRoot_UpdateManager, GrowthManager_Destroy);
        if (SolarRoot_InitManager(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    gGrowthHazardManager = r;
    return r;
}
