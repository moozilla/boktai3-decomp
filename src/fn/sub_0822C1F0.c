#include "global.h"
#include "runtime.h"
struct S {
    u8 p[0x19];
    u8 b;
    u8 q[6];
    u16 h;
};
extern struct S *gSolarController;
void Solar_StartMeasurement(void);
void Solar_ResumeMeasurement(void)
{
    struct S *s = gSolarController;
    if (s) {
        if (s->b != 0) {
            s->b = 1;
            gSolarController->h = 0;
            Solar_StartMeasurement();
        }
        gSolarPaused = 0;
    }
}
