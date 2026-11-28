#include "effects/bursts/fragment_burst.h"
#include "scene/collision/packed_contact_vector.h"

void Collider_SetAttackProperties(struct ContactCollider *, u32, u32, u32, u32, u32);
void FragmentBurst_OnContact(void);

// confidence=low: sets up the fragment burst's attack collider at +0x100.
void sub_081281D0(struct FragmentBurst *burst, u32 a, u32 b, u32 c, u32 d)
{
    struct ContactCollider *collider;
    struct PackedContactVector size, offset;

    size.x = 0x80;
    size.y = 0x20;
    size.z = 0x80;
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;
    collider = &burst->collider;
    Collider_Init(collider, 0, 0x2001, 0, 0x10, &size, &offset);
    Collider_SetAttackProperties(collider, a, b, 0, c, d);
    Collider_SetCallback(collider, FragmentBurst_OnContact, burst);
}
