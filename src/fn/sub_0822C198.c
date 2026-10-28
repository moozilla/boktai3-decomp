#include "global.h"
#include "ram.h"
#include "runtime.h"
s32 Solar_GetMeasurement(void);
s32 Solar_CaptureBaseline(void)
{
    if (gSolarController[0x19] == 2) {
        s32 v = Solar_GetMeasurement();
        if (v >= 0) {
            s32 w = v - 2;
            *(s32 *)(gSessionState + 4) = w;
            return 1;
        }
    }
    return 0;
}
