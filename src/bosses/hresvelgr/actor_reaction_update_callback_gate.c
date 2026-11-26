#include "engine/shared/global.h"
#include "player/player_slot_status.h"
#include "bosses/hresvelgr/hresvelgr_travel.h"

void SceneActors_RunStateCallback(u32);

void ActorReaction_UpdateCallbackGate(struct HresvelgrTravelActor *actor)
{
    struct HresvelgrTravelActor *saved = actor;
    actor->finished = 0;
    if (!PlayerSlot_ReadStatus(0, 4)) {
        u32 one = 1;
        if (actor->behaviorFlags & one) {
            actor->flags = 0;
            actor->finished = one;
        } else {
            SceneActors_RunStateCallback((u32)saved);
        }
    }
}
