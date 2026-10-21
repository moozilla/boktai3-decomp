#include "global.h"
extern u32 gUnk_02000258;
void GrowthHazard_CreateManager(void);
u32 GrowthHazard_InitEntryFromScript(void);
u32 GrowthHazard_AddFromScript(void)
{
    if (gUnk_02000258 == 0)
        GrowthHazard_CreateManager();
    return GrowthHazard_InitEntryFromScript();
}
