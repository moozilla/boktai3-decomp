#include "global.h"
#include "ram.h"
extern u8 *gUnk_030053F8;
s32 Solar_GetMeasurement(void);
s32 Solar_CaptureBaseline(void)
{
    if (gSolarController[0x19] == 2) {
        s32 v = Solar_GetMeasurement();
        if (v >= 0) {
            s32 w = v - 2;
            *(s32 *)(gUnk_030053F8 + 4) = w;
            return 1;
        }
    }
    return 0;
}
