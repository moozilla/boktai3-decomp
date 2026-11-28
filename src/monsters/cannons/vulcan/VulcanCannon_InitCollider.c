#include "global.h"
#include "monsters/cannons/vulcan_cannon_worker.h"
#include "scene/collision/contact_actor.h"
#include "engine/engine.h"
void VulcanCannon_ApplyHitReaction(void);

void VulcanCannon_InitCollider(struct VulcanCannon *p)
{
    struct ContactPackedVector size, offset;
    struct ContactCollider *collider = &p->collider;

    size.x = 100;
    size.y = 120;
    size.z = 100;
    offset.x = 0;
    offset.y = 128;
    offset.z = 0;
    Collider_Init(collider, 0, 0x5009, 0, 0x10, &size, &offset);
    Collider_SetPosition(collider, &p->display.position, 0);
    Collider_SetCallback(collider, VulcanCannon_ApplyHitReaction, p);
    Collider_SetBodyProperties(collider, 0, (u16)p->bodyFlags, (u16)p->bodyPower);
    Collider_Refresh(collider);
    collider->flags |= 4;
}
