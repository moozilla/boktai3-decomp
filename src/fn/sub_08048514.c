#include "wireless_link_indicator.h"
s32 WirelessLinkIndicator_Destroy(struct WirelessLinkIndicator *indicator)
{
    Anim_Remove(&indicator->animation);
    gWirelessLinkIndicator = NULL;
    return 0;
}
