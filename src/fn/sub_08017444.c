#include "global.h"

extern u8 *gWeatherState;

u32 Weather_IsActive(void)
{
    if (gWeatherState != 0) {
        if (gWeatherState[0x26] == 0) {
            return 1;
        }
    }
    return 0;
}
