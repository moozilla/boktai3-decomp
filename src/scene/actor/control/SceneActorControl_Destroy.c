#include "scene/actor/scene_actor_control.h"
#include "engine/ram.h"
u32 SceneActorControl_Destroy(struct SceneActorControlManager *p)
{
    s32 i;
    for (i = 0; i < 8; i++) {
        if (SceneActorControl_IsUsed(p, i)) {
            struct SceneActorControlSlot *slot = &p->slots[i];
            if (slot->active)
                Sprite_Remove(slot);
            Collider_Remove(&slot->collider);
        }
    }
    return (u32)(gSceneActorControlManager = 0);
}
