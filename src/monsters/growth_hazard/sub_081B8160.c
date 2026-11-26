#include "global.h"
#include "engine/ram.h"
void *GrowthHazard_CreateManager(void);
u32 GrowthHazard_InitEntryFromScript(void *);
u32 GrowthHazard_AddFromScript(void)
{
    void *manager = gGrowthHazardManager;
    if (manager == 0)
        manager = GrowthHazard_CreateManager();
    return GrowthHazard_InitEntryFromScript(manager);
}
