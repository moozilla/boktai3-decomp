#ifndef GUARD_WIRELESS_LINK_INDICATOR_H
#define GUARD_WIRELESS_LINK_INDICATOR_H
#include "projected_display.h"
#include "rfu_compat.h"
struct WirelessLinkIndicator {
    struct Task task;
    u32 frame, animationTick, selectedFrame;
    struct ProjectedDisplayResource resource;
    struct ProjectedDisplayEntry animation;
};
/* 0x020000F4 */
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
void WirelessLinkIndicator_UpdateChildStatus(struct WirelessLinkIndicator *);
void WirelessLinkIndicator_SetPosition(u32, u32);
#endif
