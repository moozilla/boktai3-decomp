#include "global.h"
extern u32 gUnk_030035C0, gUnk_03006A38, gUnk_030035B0, gUnk_030035B4;
extern s32 gUnk_03006A48;
void Solar_Enable(void);
void Solar_StartMeasurement(void)
{
    if (gUnk_030035C0 == 0)
        Solar_Enable();
    gUnk_03006A38 = 1;
    gUnk_030035B0 = 0;
    gUnk_030035B4 = 0;
    gUnk_03006A48 = -1;
}
