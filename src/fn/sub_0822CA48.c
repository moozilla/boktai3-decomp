#include "global.h"
#include "ram.h"

void Solar_StopMeasurement(void);

u32 Solar_DestroyController(void)
{
    Solar_StopMeasurement();
    gSolarController = 0;
    return 0;
}
