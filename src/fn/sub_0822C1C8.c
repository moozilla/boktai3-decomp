#include "global.h"
#include "runtime.h"
struct S {
    u8 p[0x19];
    u8 b;
};
extern struct S *gSolarController;
void Solar_StopMeasurement(void);
void Solar_PauseMeasurement(void)
{
    if (gSolarController) {
        if (gSolarController->b)
            Solar_StopMeasurement();
        gSolarPaused = 1;
    }
}
