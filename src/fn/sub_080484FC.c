#define WirelessLinkIndicator_UpdateLinkStatus sub_0804846C
#include "wireless_link_indicator.h"
#define WirelessLinkIndicator_Update sub_080484FC
s32 WirelessLinkIndicator_Update(struct WirelessLinkIndicator *indicator)
{
    WirelessLinkIndicator_UpdateLinkStatus(indicator);
    ++indicator->frame;
    return 0;
}
