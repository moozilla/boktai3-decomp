#include "wireless_link_indicator.h"
#define WirelessLinkIndicator_UpdateChildStatus sub_08048438
void WirelessLinkIndicator_UpdateChildStatus(struct WirelessLinkIndicator *indicator)
{
    struct RfuLinkStatus *status = gRfuLinkStatus;
    if (!status->parentChild) {
        WirelessLinkIndicator_SelectStrengthFrame(indicator, status->strength[0]);
        indicator->animation.flags &= ~1;
    } else
        indicator->animation.flags |= 1;
}
