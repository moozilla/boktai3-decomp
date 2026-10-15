#include "proximity_icon.h"
u32 ProximityIcon_Destroy(struct ProximityIcon *icon)
{
    Anim_Remove(&icon->animation);
    return 0;
}
