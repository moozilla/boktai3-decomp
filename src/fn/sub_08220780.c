#define Collider_TestSegmentBounds sub_08220780
#define Collider_TestSegmentXAxis  sub_08220498
#define Collider_TestSegmentYAxis  sub_08220590
#define Collider_TestSegmentZAxis  sub_08220688
#include "global.h"
#include "contact_actor.h"
u32 sub_08220498(const struct ContactPosition *, const struct ContactPosition *, const s16 *,
                 const struct ContactPosition *, void *);
u32 sub_08220590(const struct ContactPosition *, const struct ContactPosition *, const s16 *,
                 const struct ContactPosition *, void *);
u32 sub_08220688(const struct ContactPosition *, const struct ContactPosition *, const s16 *,
                 const struct ContactPosition *, void *);
u32 sub_08220780(const struct ContactPosition *start, const struct ContactPosition *end,
                 struct ContactCollider *collider, void *output)
{
    const s16 *center = &collider->x;
    const struct ContactPosition *bounds = &collider->bounds;
    if (sub_08220498(start, end, center, bounds, output))
        return 1;
    if (sub_08220590(start, end, center, bounds, output))
        return 1;
    if (sub_08220688(start, end, center, bounds, output))
        return 1;
    return 0;
}
