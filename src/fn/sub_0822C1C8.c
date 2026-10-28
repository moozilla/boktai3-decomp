#include "global.h"
#include "runtime.h"
#include "ram.h"
struct S {
    u8 p[0x19];
    u8 b;
};
void Solar_StopMeasurement(void);
void Solar_PauseMeasurement(void)
{
    if (((struct S *)gSolarController)) {
        if (((struct S *)gSolarController)->b)
            Solar_StopMeasurement();
        gSolarPaused = 1;
    }
}
