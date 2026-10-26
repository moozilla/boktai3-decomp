#include "global.h"
#include "ram.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 GrowthHazard_InitManager(void *);
void Task_Kill(void *);
void GrowthHazard_UpdateManager(void);
void sub_081B7D44(void);

void *GrowthHazard_CreateManager(void)
{
    void *r = Task_Create(8, 0x4c0);
    if (r != 0) {
        Task_SetCallbacks(r, GrowthHazard_UpdateManager, sub_081B7D44);
        if (GrowthHazard_InitManager(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    gGrowthHazardManager = r;
    return r;
}
