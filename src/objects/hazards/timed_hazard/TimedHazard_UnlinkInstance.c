#include "global.h"
#include "engine/ram.h"
#include "objects/hazards/timed_hazard.h"

u32 TimedHazard_UnlinkInstance(struct TimedHazardPool *pool, struct TimedHazard *hazard)
{
    struct TimedHazard *previous = hazard->previous;
    struct TimedHazard *next = hazard->next;

    if (previous)
        previous->next = next;
    else
        pool->head = next;
    if (next)
        next->previous = previous;
    return 0;
}
