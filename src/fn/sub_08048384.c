#include "contact_burst.h"
void ContactBurst_SetEnabled(struct ContactBurst *effect, u32 enabled)
{
    if (effect)
        effect->enabled = enabled;
}
