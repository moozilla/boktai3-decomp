#include "global.h"

extern u8 *gUnk_03002604;
void Solar_StopMeasurement(void);

u32 Solar_DestroyController(void)
{
    Solar_StopMeasurement();
    gUnk_03002604 = 0;
    return 0;
}
