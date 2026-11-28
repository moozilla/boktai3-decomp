#include "effects/bursts/fragment_effect.h"

void FragmentEffect_UpdateCollider(struct Fragment *fragment)
{
    Collider_SetPosition(&fragment->collider, &fragment->sprite.position, 0);
    Collider_Refresh(&fragment->collider);
}
