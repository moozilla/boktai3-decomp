#include "global.h"
#include "ram.h"
int Weather_EnterPhase(void *, int);
int Weather_Start(void)
{
    u8 *p = gWeatherState;
    if (!p)
        return -1;
    Weather_EnterPhase(p, 1);
    p[32] = 1;
}
