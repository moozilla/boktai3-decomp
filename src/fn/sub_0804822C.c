#include "contact_burst.h"
s32 ContactBurst_Destroy(struct ContactBurst *effect)
{
    Collider_Remove(&effect->collider);
    Sprite_Remove(&effect->sprite);
    return 0;
}
