#include "global.h"
#include "proximity_actor.h"
#include "engine.h"
void sub_08105808(void);
void ProximityActor_InitializeCollider(struct ProximityActor *actor)
{
    struct ColliderVector {
        s16 x, y, z;
    } size, offset;
    if (actor->variant == 1) {
        size.x = 0x56;
        size.y = 0x80;
        size.z = 0x56;
        offset.x = 0;
        offset.y = 0x80;
        offset.z = 0;
    } else {
        size.x = 0x60;
        size.y = 0x10;
        size.z = 0x60;
        offset.x = 0;
        offset.y = 0x10;
        offset.z = 0;
    }
    Collider_Init(actor->collider, 0, 0x4003, 0, 16, &size, &offset);
    Collider_SetPosition(actor->collider, &actor->colliderPosition, 0);
    Collider_SetCallback(actor->collider, sub_08105808, actor);
    Collider_Refresh(actor->collider);
}
