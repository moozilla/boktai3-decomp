#include "global.h"
#include "ram.h"

void Weather_ClearEffects(void);

u32 Weather_Destroy(void)
{
    Weather_ClearEffects();
    gWeatherState = 0;
    return 0;
}
