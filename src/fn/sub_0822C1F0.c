#include "global.h"
#include "runtime.h"
#include "ram.h"
struct S {
    u8 p[0x19];
    u8 b;
    u8 q[6];
    u16 h;
};
void Solar_StartMeasurement(void);
void Solar_ResumeMeasurement(void)
{
    struct S *s = ((struct S *)gSolarController);
    if (s) {
        if (s->b != 0) {
            s->b = 1;
            ((struct S *)gSolarController)->h = 0;
            Solar_StartMeasurement();
        }
        gSolarPaused = 0;
    }
}
