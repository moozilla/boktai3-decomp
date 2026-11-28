#include "global.h"
#include "scene/collision/contact_collider.h"

void Collision_AccumulateNearbyResponse(struct ContactCollider *source,
                                        struct ContactCollider *target, s32 amount)
{
    if ((target->flags & 0x100) && (source->flags & 0x100)) {
        s32 delta = target->direction - source->direction;
        if (delta < 0)
            delta = -delta;
        if (delta <= 32) {
            target->cost += (amount << 4) >> 6;
            target->costFlags |= 8;
        }
    }
}
