#include "global.h"
extern u32 gUnk_02000258;
void GrowthHazard_CreateManager(void);
u32 sub_081B7E4C(void);
u32 GrowthHazard_AddFromScript(void)
{
    if (gUnk_02000258 == 0)
        GrowthHazard_CreateManager();
    return sub_081B7E4C();
}
