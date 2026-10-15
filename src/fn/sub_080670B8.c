#include "scene_actor_control.h"
#define SceneActorControl_InitCollider sub_080670B8
void sub_080670A0(void *, void *, struct SceneActorControlSlot *);
void SceneActorControl_InitCollider(struct SceneActorControlSlot *slot)
{
    struct ContactPackedVector bounds, offset;
    struct ContactCollider *collider;
    bounds.x = 0x80;
    bounds.y = 0x80;
    bounds.z = 0x52;
    offset.x = 0x40;
    offset.y = 0x80;
    offset.z = 0x2A;
    collider = &slot->collider;
    Collider_Init(collider, 0, 0x4001, 0, 0x10, &offset, &bounds);
    Collider_SetPosition(collider, &slot->savedPosition, 0);
    Collider_SetCallback(collider, sub_080670A0, slot);
    Collider_Refresh(collider);
}
