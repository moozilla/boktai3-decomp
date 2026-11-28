#include "global.h"
#include "link/session/room_actor.h"
extern void *gUnk_08605F90[];
void Centipede_InitStateHandlers(struct RoomActor *actor)
{
    actor->behaviorCallbacks = gUnk_08605F90;
}
