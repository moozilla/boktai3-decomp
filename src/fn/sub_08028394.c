#include "scene_actors.h"
#include "player_slot_status.h"

/* Run the actor's state callback unless scene events are paused. */
void SceneActors_RunUnpausedState(struct StateActor *p)
{
    p->actionFinished = 0;
    if (!PlayerSlot_ReadStatus(0, 4))
        SceneActors_RunStateCallback(p);
}
