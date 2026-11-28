#include "monsters/cannons/plasma_cannon.h"
#include "scene/collision/collision_response.h"
#include "engine/engine.h"
#include "scene/collision/contact_actor.h"
void PlasmaCannon_InitCollider(struct OrangeSpineActor *p)
{
    struct ContactPackedVector size, offset;
    struct ContactCollider *q = &p->collider;
    size.x = 100;
    size.y = 120;
    size.z = 100;
    offset.x = 0;
    offset.y = 120;
    offset.z = 0;
    Collider_Init(q, 0, 0x5009, 0, 0x10, &size, &offset);
    Collider_SetPosition(q, &p->sprite.display.position, 0);
    Collider_SetCallback(q, PlasmaCannon_ApplyHitReaction, p);
    Collider_SetBodyProperties(q, 0, p->bodyReactionFlags, p->sparkMask);
    Collider_Refresh(q);
    q->flags |= 4;
}
