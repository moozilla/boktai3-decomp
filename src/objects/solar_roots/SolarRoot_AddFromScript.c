#include "global.h"
#include "engine/ram.h"
void *SolarRoot_CreateManager(void);
u32 SolarRoot_InitEntryFromScript(void *);
u32 SolarRoot_AddFromScript(void)
{
    void *manager = gGrowthHazardManager;
    if (manager == 0)
        manager = SolarRoot_CreateManager();
    return SolarRoot_InitEntryFromScript(manager);
}
