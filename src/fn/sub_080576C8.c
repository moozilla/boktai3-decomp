#include "proximity_icon.h"
#define ProximityIcon_Update sub_080576C8
s32 ProximityIcon_Update(struct ProximityIcon *icon)
{
    if (!(gUnk_030053F4 & 0x200) && sub_08057678(icon))
        icon->animation.flags &= ~1;
    else
        icon->animation.flags |= 1;
    return 0;
}
