#include "monsters/centipede/centipede_body.h"

void Centipede_UpdateOscillation(struct ChaseActor *actor, s32 limit, s32 scale)
{
    struct TentacleBody *body = actor->base.context;
    if (body->oscillationPhase >= limit) {
        body->oscillationPhase = limit;
        body->oscillationDirection = -1;
    } else if (body->oscillationPhase <= -limit) {
        body->oscillationPhase = -limit;
        body->oscillationDirection = 1;
    }
    body->oscillationPhase += body->oscillationDirection * scale;
}
