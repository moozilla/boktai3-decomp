#include "global.h"
/* Solar_IsMeasurementReady: Checks that the solar task is in measurement state and is not paused */
#define Solar_IsMeasurementReady sub_0822C154
extern u8 *gUnk_03002604;
extern u16 gUnk_030054BC;
u32 sub_0822C154(void)
{
    u8 *p = gUnk_03002604;
    if (p && !gUnk_030054BC && p[0x19] == 2)
        return 1;
    return 0;
}
