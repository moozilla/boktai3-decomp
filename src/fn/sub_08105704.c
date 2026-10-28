#define ProximityActor_TestFlags Pool184_TestStatusFlags
#define ProximityActor_SetFlags  Pool184_AddStatusFlags
#include "global.h"
#include "proximity_actor.h"
u8 Pool184_TestStatusFlags(struct ProximityActor *, u32);
void Pool184_AddStatusFlags(struct ProximityActor *, u16);
void ProximityActor_SetState(struct ProximityActor *, s32);
void Sound_PlaySong(u32);
void ProximityActor_CheckPlayerRange(struct ProximityActor *actor)
{
    struct SceneVec delta;
    s32 distance;
    if (!Pool184_TestStatusFlags(actor, 1)) {
        ProximityActor_Subtract(&delta, &actor->position, &gPlayers[0]->position);
        distance = delta.x * delta.x + delta.z * delta.z;
        actor->distanceSquared = distance;
        if (actor->distanceSquared < actor->triggerDistanceSquared) {
            ProximityActor_SetState(actor, 2);
            Pool184_AddStatusFlags(actor, 1);
            Sound_PlaySong(0x3C7);
        }
    }
}
