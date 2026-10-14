#include "wireless_link_indicator.h"
s32 WirelessLinkIndicator_Update(struct WirelessLinkIndicator *indicator)
{
    WirelessLinkIndicator_UpdateLinkStatus(indicator);
    ++indicator->frame;
    return 0;
}
