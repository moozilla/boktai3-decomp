#include "wireless_link_indicator.h"
#define WirelessLinkIndicator_Get sub_080483A0
struct WirelessLinkIndicator *WirelessLinkIndicator_Get(void)
{
    return gUnk_020000F4;
}
