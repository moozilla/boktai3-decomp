#include "monsters/centipede/centipede_body.h"


void Centipede_ResetSegmentPositions(struct TentacleBody *p, struct ContactPosition *pos)
{
    s32 i;
    for (i = 0; i < p->segmentCount; i++) {
        p->segments[i].position = *pos;
        p->segments[i].targetPosition = *pos;
    }
}
