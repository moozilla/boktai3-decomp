#include "global.h"
extern u8 *gUnk_02000214;
void Hud_DrawStoredEnergyGauge(u8 *, s32);

void sub_0817845C(void)
{
    if (gUnk_02000214 != NULL)
        Hud_DrawStoredEnergyGauge(gUnk_02000214, 2);
}
