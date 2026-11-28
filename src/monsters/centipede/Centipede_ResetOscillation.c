#include "monsters/centipede/centipede_body.h"
void Centipede_ResetOscillation(struct ChaseActor *actor)
{
    struct TentacleBody *body = actor->base.context;
    body->oscillationPhase = 0;
    body->oscillationDirection = 1;
}
