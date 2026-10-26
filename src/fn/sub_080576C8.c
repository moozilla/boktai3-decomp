#include "proximity_icon.h"
s32 ProximityIcon_Update(struct ProximityIcon *icon)
{
    if (!(gEngineFlags & 0x200) && ProximityIcon_IsPlayerNearby(icon))
        icon->animation.flags &= ~1;
    else
        icon->animation.flags |= 1;
    return 0;
}
