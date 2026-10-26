#include "global.h"
#include "ram.h"
int Weather_EnterPhase(void *, int);
int Weather_Stop(void)
{
    u8 *p = gWeatherState;
    int state;
    if (!p)
        return -1;
    state = p[29];
    switch (state) {
        case 1:
        case 2:
        case 3:
            Weather_EnterPhase(p, 4);
            break;
    }
    p[32] = 0;
}
