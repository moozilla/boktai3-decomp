#include "global.h"
#include "engine/ram.h"
#include "objects/hazards/timed_hazard.h"

void TimedHazard_UpdateActivationDelay(struct TimedHazard *hazard)
{
    if (hazard->timer++ > hazard->delay) {
        u32 active = 1;
        hazard->state = active;
        hazard->timer = 0;
        hazard->entered = active;
    }
}
