#ifndef GUARD_WIRELESS_LINK_INDICATOR_H
#define GUARD_WIRELESS_LINK_INDICATOR_H
#include "projected_display.h"
#include "rfu_compat.h"
struct WirelessLinkIndicator {
    u8 task[0x18];
    u32 frame, animationTick, selectedFrame;
    struct ProjectedDisplayResource resource;
    struct ProjectedDisplayEntry animation;
};
extern struct WirelessLinkIndicator *gWirelessLinkIndicator;
void WirelessLinkIndicator_SelectStrengthFrame(struct WirelessLinkIndicator *, s32);
void WirelessLinkIndicator_UpdateLinkStatus(struct WirelessLinkIndicator *);
s32 WirelessLinkIndicator_Initialize(struct WirelessLinkIndicator *);
s32 WirelessLinkIndicator_Update(struct WirelessLinkIndicator *);
s32 WirelessLinkIndicator_Destroy(struct WirelessLinkIndicator *);
struct WirelessLinkIndicator *WirelessLinkIndicator_Get(void);
struct WirelessLinkIndicator *WirelessLinkIndicator_Create(void);
u32 Wireless_GetStatus(void);
s32 Wireless_IsSessionConnected(void);
s32 Wireless_GetConnectedParentSlot(void);
#endif
