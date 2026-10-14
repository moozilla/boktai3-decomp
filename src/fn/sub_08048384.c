#include "contact_burst.h"
#define ContactBurst_SetEnabled sub_08048384
void ContactBurst_SetEnabled(struct ContactBurst *effect, u32 enabled)
{
    if (effect)
        effect->enabled = enabled;
}
