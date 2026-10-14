#include "wireless_link_indicator.h"
void WirelessLinkIndicator_SetPosition(u32 x, u32 y)
{
    struct WirelessLinkIndicator *indicator = WirelessLinkIndicator_Create();
    if (indicator) {
        struct ProjectedDisplayEntry *animation = &indicator->animation;
        animation->position.x = x;
        animation->position.y = y;
    }
}
