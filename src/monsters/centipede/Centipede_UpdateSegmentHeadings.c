#include "monsters/centipede/centipede_body.h"


static inline u8 TestMask(u8 *flags, u32 mask)
{
    if (*flags & mask)
        return TRUE;
    return FALSE;
}

static inline void VecSub(struct ContactPosition *d, struct ContactPosition *a, struct ContactPosition *b)
{
    d->x = a->x - b->x;
    d->y = a->y - b->y;
    d->z = a->z - b->z;
}

void Centipede_UpdateSegmentHeadings(struct RoomActor *actor)
{
    struct TentacleBody *g = actor->context;
    struct ContactPosition d;
    struct ContactPosition prev = actor->body.position;
    s32 i;
    for (i = 0; i < g->segmentCount; i++) {
        if (!TestMask(&g->segments[i].animation.status, 1)) {
            VecSub(&d, &prev, &g->segments[i].position);
            g->segments[i].animation.direction = Math_Atan2(d.x, d.z);
            prev = g->segments[i].position;
        }
    }
}
