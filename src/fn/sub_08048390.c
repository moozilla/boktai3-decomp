#include "contact_burst.h"
#define ContactBurst_GetEnabled sub_08048390
u32 ContactBurst_GetEnabled(struct ContactBurst *effect)
{
    if (!effect)
        return 0;
    return effect->enabled;
}
