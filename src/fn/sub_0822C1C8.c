#include "global.h"
struct S {
    u8 p[0x19];
    u8 b;
};
extern struct S *gUnk_03002604;
extern u16 gUnk_030054BC;
void Solar_StopMeasurement(void);
void Solar_PauseMeasurement(void)
{
    if (gUnk_03002604) {
        if (gUnk_03002604->b)
            Solar_StopMeasurement();
        gUnk_030054BC = 1;
    }
}
