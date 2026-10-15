#include "proximity_icon.h"
#define ProximityIcon_Destroy sub_08057704
u32 ProximityIcon_Destroy(struct ProximityIcon *icon)
{
    Anim_Remove(&icon->animation);
    return 0;
}
