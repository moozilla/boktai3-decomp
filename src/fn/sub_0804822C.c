#include "contact_burst.h"
#define ContactBurst_Destroy sub_0804822C
s32 ContactBurst_Destroy(struct ContactBurst *effect)
{
    Collider_Remove(&effect->collider);
    Sprite_Remove(&effect->sprite);
    return 0;
}
