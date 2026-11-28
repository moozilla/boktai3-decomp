#include "global.h"
#include "monsters/garmr_statue.h"
#include "scene/collision/contact_actor.h"
#include "engine/engine.h"
void GarmrStatue_ApplyHitReaction(void);

void GarmrStatue_InitCollider(struct GarmrStatue *p)
{
    struct ContactPackedVector size, offset;
    struct ContactCollider *collider = &p->collider;

    size.x = 100;
    size.y = 120;
    size.z = 100;
    offset.x = 0;
    offset.y = 120;
    offset.z = 0;
    Collider_Init(collider, 0, 0x5009, 0, 0x10, &size, &offset);
    Collider_SetPosition(collider, &p->sprite.display.position, 0);
    Collider_SetCallback(collider, GarmrStatue_ApplyHitReaction, p);
    Collider_SetBodyProperties(collider, 0, (u16)p->bodyReactionFlags, (u16)p->sparkMask);
    Collider_Refresh(collider);
    collider->flags |= 4;
}
