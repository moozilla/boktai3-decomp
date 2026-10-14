#include "contact_burst.h"
u32 ContactBurst_GetEnabled(struct ContactBurst *effect)
{
    if (!effect)
        return 0;
    return effect->enabled;
}
