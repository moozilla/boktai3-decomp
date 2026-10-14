#include "wireless_link_indicator.h"
s32 WirelessLinkIndicator_Destroy(struct WirelessLinkIndicator *indicator)
{
    Anim_Remove(&indicator->animation);
    gUnk_020000F4 = NULL;
    return 0;
}
