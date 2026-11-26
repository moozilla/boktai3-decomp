#include "field/weather_and_storm_controller/weather_state.h"

void Weather_EnterPhase(struct WeatherState *weather, u32 phase)
{
    weather->phase = phase;
    weather->phaseTimer = 0;
    weather->entered = 1;
}
