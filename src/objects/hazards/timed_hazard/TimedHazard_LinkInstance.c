#include "global.h"
#include "engine/ram.h"
#include "objects/hazards/timed_hazard.h"

u32 TimedHazard_LinkInstance(struct TimedHazardPool *pool, struct TimedHazard *hazard)
{
    hazard->previous = 0;
    hazard->next = pool->head;
    if (hazard->next)
        hazard->next->previous = hazard;
    pool->head = hazard;
    return 0;
}
