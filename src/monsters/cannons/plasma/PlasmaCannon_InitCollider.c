#include "monsters/cannons/plasma_cannon.h"
#include "scene/collision/collision_response.h"
#include "engine/engine.h"
#include "scene/collision/packed_contact_halfwords.h"
#include "scene/collision/packed_collider_extents.h"
void PlasmaCannon_InitCollider(struct OrangeSpineActor *p)
{
    struct PackedColliderExtents a, b;
    struct ContactCollider *q = &p->collider;
    a.xy.lo = 100;
    a.xy.hi = 120;
    a.zAndPad.lo = 100;
    *(u32 *)&b.xy = 0x780000;
    *(u32 *)&b.zAndPad &= 0xffff0000;
    Collider_Init(q, 0, 0x5009, 0, 0x10, &a, &b);
    Collider_SetPosition(q, &p->sprite.display.position, 0);
    Collider_SetCallback(q, PlasmaCannon_ApplyHitReaction, p);
    Collider_SetBodyProperties(q, 0, p->bodyReactionFlags, p->sparkMask);
    Collider_Refresh(q);
    q->flags |= 4;
}
