#include "global.h"
#include "proximity_actor.h"
s32 ProximityActor_IsPlayerInsideOriginRange(struct ProximityActor *actor)
{
    struct SceneVec delta;
    ProximityActor_Subtract(&delta, &actor->origin, &gPlayers[0]->position);
    actor->previousDistanceSquared = actor->distanceSquared;
    actor->distanceSquared = delta.x * delta.x + delta.z * delta.z;
    if (actor->distanceSquared < actor->triggerDistanceSquared)
        return 1;
    return 0;
}
