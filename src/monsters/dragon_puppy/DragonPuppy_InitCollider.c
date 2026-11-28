#include "global.h"
#include "monsters/dragon_puppy.h"
#include "scene/collision/contact_actor.h"
#include "engine/engine.h"
void DragonPuppy_ApplyHitReaction(void);

void DragonPuppy_InitCollider(struct DragonPuppy *p)
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
    Collider_SetPosition(collider, &p->sprite.position, 0);
    Collider_SetCallback(collider, DragonPuppy_ApplyHitReaction, p);
    Collider_SetBodyProperties(collider, 0, (u16)p->bodyReactionFlags, (u16)p->sparkMask);
    Collider_Refresh(collider);
    collider->flags |= 4;
}
