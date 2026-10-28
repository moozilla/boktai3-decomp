#include "global.h"
#include "ram.h"

u32 Weather_IsActive(void)
{
    if (((u8 *)gWeatherState) != 0) {
        if (((u8 *)gWeatherState)[0x26] == 0) {
            return 1;
        }
    }
    return 0;
}
