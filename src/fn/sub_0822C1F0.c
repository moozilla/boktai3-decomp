#include "global.h"
struct S {
    u8 p[0x19];
    u8 b;
    u8 q[6];
    u16 h;
};
extern struct S *gUnk_03002604;
extern u16 gUnk_030054BC;
void Solar_StartMeasurement(void);
void Solar_ResumeMeasurement(void)
{
    struct S *s = gUnk_03002604;
    if (s) {
        if (s->b != 0) {
            s->b = 1;
            gUnk_03002604->h = 0;
            Solar_StartMeasurement();
        }
        gUnk_030054BC = 0;
    }
}
