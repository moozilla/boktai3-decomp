#include "global.h"
#include "link/session/room_actor.h"
/* CentipedeBody_ClearStateEntryFlag: Clears the pending state-entry byte when set */

void CentipedeBody_ClearStateEntryFlag(struct RoomActor *actor)
{
    if (actor->field2B2)
        actor->field2B2 = 0;
}
