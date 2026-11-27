#include "monsters/cannons/plasma_cannon.h"
#include "scene/collision/collision_response.h"
#include "engine/engine.h"
#include "scene/collision/packed_contact_halfwords.h"
struct Z {
    struct PackedContactHalfwords a, b;
};
void PlasmaCannon_InitCollider(struct OrangeSpineActor *p)
{
    struct Z a, b;
    struct ContactCollider *q = &p->collider;
    a.a.lo = 100;
    a.a.hi = 120;
    a.b.lo = 100;
    *(u32 *)&b.a = 0x780000;
    *(u32 *)&b.b &= 0xffff0000;
    Collider_Init(q, 0, 0x5009, 0, 0x10, &a, &b);
    Collider_SetPosition(q, &p->sprite.display.position, 0);
    Collider_SetCallback(q, PlasmaCannon_ApplyHitReaction, p);
    Collider_SetBodyProperties(q, 0, p->bodyReactionFlags, p->sparkMask);
    Collider_Refresh(q);
    q->flags |= 4;
}
