#include "wireless_link_indicator.h"
/* WirelessLinkIndicator_UpdateChildStatus: Updates the indicator from child strength slot zero and shows it in child mode; hides it in parent mode */
#define WirelessLinkIndicator_UpdateChildStatus sub_08048438
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
