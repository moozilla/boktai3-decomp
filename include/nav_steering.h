#ifndef GUARD_NAV_STEERING_H
#define GUARD_NAV_STEERING_H

#include "global.h"

struct NavSteeringPosition {
    s16 x, y, z;
};

struct NavSteeringProbe {
    u16 flags;
    u8 pad[0x1A];
    struct NavSteeringPosition pos;
};

void NavMesh_UpdateProbe(struct NavSteeringProbe *probe, struct NavSteeringPosition *target);
s32 Math_Atan2(s32, s32);
s32 Actor_MoveAlongAngle(struct NavSteeringPosition *pos, u32 angle, s32 dist);

static inline void NavSteering_Subtract(struct NavSteeringPosition *out, struct NavSteeringPosition *a, struct NavSteeringPosition *b)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}

/* Both four- and eight-way probe updates mark this flag after blocked movement. */
#define NAV_STEERING_MARK_BLOCKED(probe_)                                                           \
    do {                                                                                           \
        s32 flags = (probe_)->flags | 2;                                                           \
        (probe_)->flags = flags;                                                                   \
    } while (0)

#endif
