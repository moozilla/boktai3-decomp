#define ProximityActor_IsPlayerInsideOriginRange sub_08105790
#define gPlayers                                 gUnk_02000580
#include "global.h"
#include "proximity_actor.h"
s32 sub_08105790(struct ProximityActor *actor)
{
    struct SceneVec delta;
    ProximityActor_Subtract(&delta, &actor->origin, &gUnk_02000580[0]->position);
    actor->previousDistanceSquared = actor->distanceSquared;
    actor->distanceSquared = delta.x * delta.x + delta.z * delta.z;
    if (actor->distanceSquared < actor->triggerDistanceSquared)
        return 1;
    return 0;
}
