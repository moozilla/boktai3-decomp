#include "scene/actor/scene_actor_control.h"
#include "engine/ram.h"
/* SceneActorControl_RequestAllDepartures: Requests departure and sets collider flags for every occupied slot not already departing */
void SceneActorControl_RequestAllDepartures(void)
{
    struct SceneActorControlManager *p = gSceneActorControlManager;
    if (p) {
        s32 i;
        for (i = 0; i < 8; i++) {
            if (SceneActorControl_IsUsed(p, i)) {
                struct SceneActorControlSlot *slot = &p->slots[i];
                if (slot->index >= 0 && slot->kind != 2) {
                    slot->flags |= 2;
                    slot->collider.flags |= 4;
                }
            }
        }
    }
}
