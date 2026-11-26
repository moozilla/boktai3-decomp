#include "engine/shared/scene_object_slot.h"

void Camera_RequestShake(int);

void SceneObject_UpdateLowered(struct SceneObjSlot *slot)
{
    s16 timer = slot->timer;
    if (timer) {
        Camera_RequestShake((s16)slot->timer);
        slot->timer--;
    } else if (slot->loweredScript) {
        Script_Call(slot->loweredScript, NULL);
        slot->loweredScript = 0;
    }
}
