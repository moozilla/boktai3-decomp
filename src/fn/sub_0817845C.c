#include "global.h"
#include "ram.h"
void Hud_DrawStoredEnergyGauge(u8 *, s32);

void sub_0817845C(void)
{
    if (gFieldHud != NULL)
        Hud_DrawStoredEnergyGauge(gFieldHud, 2);
}
