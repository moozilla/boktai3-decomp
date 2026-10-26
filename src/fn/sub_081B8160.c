#include "global.h"
#include "ram.h"
void GrowthHazard_CreateManager(void);
u32 GrowthHazard_InitEntryFromScript(void);
u32 GrowthHazard_AddFromScript(void)
{
    if (gGrowthHazardManager == 0)
        GrowthHazard_CreateManager();
    return GrowthHazard_InitEntryFromScript();
}
