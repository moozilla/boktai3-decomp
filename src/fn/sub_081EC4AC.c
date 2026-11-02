#include "departure_burst.h"
#include "scene_debris.h"

void ActorEffect_SpawnBurstAtAnchor(struct DepartureActor *actor, u32 variant)
{
    struct ContactPosition position;
    struct ContactPosition velocity;
    position.x = actor->screen.x;
    position.y = 158;
    position.z = 50;
    velocity.x = DepartureBurst_NextBit() + 2;
    velocity.y = -DepartureBurst_NextBit();
    velocity.z = 0;
    if (variant == 0)
        sub_08113398(&position, &velocity, actor->effectResource, actor->effectGraphics, 5, 0, 0, 0,
                     2, 42, 0, 512, 1, 0);
    else
        sub_08113398(&position, &velocity, actor->effectResource, actor->effectGraphics, 6, 0, 0, 0,
                     2, 30, 0, 512, 1, 0);
}
