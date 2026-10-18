#include "global.h"
#include "contact_actor.h"
u32 Collider_TestSegmentXAxis(const struct ContactPosition *, const struct ContactPosition *,
                              const s16 *, const struct ContactPosition *, void *);
u32 Collider_TestSegmentYAxis(const struct ContactPosition *, const struct ContactPosition *,
                              const s16 *, const struct ContactPosition *, void *);
u32 Collider_TestSegmentZAxis(const struct ContactPosition *, const struct ContactPosition *,
                              const s16 *, const struct ContactPosition *, void *);
u32 Collider_TestSegmentBounds(const struct ContactPosition *start,
                               const struct ContactPosition *end, struct ContactCollider *collider,
                               void *output)
{
    const s16 *center = &collider->x;
    const struct ContactPosition *bounds = &collider->bounds;
    if (Collider_TestSegmentXAxis(start, end, center, bounds, output))
        return 1;
    if (Collider_TestSegmentYAxis(start, end, center, bounds, output))
        return 1;
    if (Collider_TestSegmentZAxis(start, end, center, bounds, output))
        return 1;
    return 0;
}
