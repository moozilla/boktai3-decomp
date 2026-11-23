#include "global.h"
#include "player_slot_status.h"
#include "state_actor.h"

void SceneActors_RunStateCallback(void *);

void BossSlot2_RunUnpausedStateCallback(struct StateActor *p)
{
    p->actionFinished = 0;
    if (!PlayerSlot_ReadStatus(0, 4))
        SceneActors_RunStateCallback(p);
}
