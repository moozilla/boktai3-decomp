#include "link/session/room_actor.h"

void SceneActorControl_OnIdle(u8 *p)
{
    struct RoomActor *actor = (struct RoomActor *)p;
    if (actor->field2B2)
        actor->field2B2 = 0;
}
