#include "global.h"
#include "ram.h"

void Weather_ClearEffects(void *);

u32 Weather_Destroy(void *weather)
{
    Weather_ClearEffects(weather);
    gWeatherState = 0;
    return 0;
}
