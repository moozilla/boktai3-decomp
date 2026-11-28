#include "link/session/room_actor.h"

void SceneActorControl_DispatchControl(struct RoomActor *actor)
{
    s32 frame;
    actor->stepDone = 0;
    frame = actor->field2C8++;
    if (actor->target)
        ((void (*)(struct RoomActor *, s32))actor->target)(actor, frame);
}
