#include "monsters/centipede/centipede_body.h"
#include "bosses/vanargand/vanargand_parts.h"


static inline u8 TestMask(u8 *flags, u32 mask)
{
    if (*flags & mask)
        return TRUE;
    return FALSE;
}

void Centipede_UpdateSegmentHeadings(struct RoomActor *actor)
{
    struct TentacleBody *g = actor->context;
    struct ContactPosition d;
    struct ContactPosition prev = actor->body.position;
    s32 i;
    for (i = 0; i < g->segmentCount; i++) {
        if (!TestMask(&g->segments[i].animation.status, 1)) {
            Vanargand_Difference(&d, &prev, &g->segments[i].position);
            g->segments[i].animation.direction = Math_Atan2(d.x, d.z);
            prev = g->segments[i].position;
        }
    }
}
