#include "monsters/centipede/centipede_body.h"
/* CentipedeBody_UpdateSegmentPositions: Periodically shifts stored segment positions and inserts the supplied position at the front */

void Centipede_SetSegmentPosition(void *, s32, void *);

void CentipedeBody_UpdateSegmentPositions(struct ChaseActor *actor, struct ContactPosition *pos)
{
    struct TentacleBody *body = actor->base.context;
    if (body->trailDelay) {
        body->trailDelay--;
        return;
    }
    body->trailDelay = 12;
    Centipede_SetSegmentPosition(body, body->segmentCount - 1,
                                 &body->segments[body->segmentCount >> 1].position);
    Centipede_SetSegmentPosition(body, body->segmentCount >> 1, &body->segments[0].position);
    Centipede_SetSegmentPosition(body, 0, pos);
}
