#ifndef GUARD_WIRELESS_LINK_INDICATOR_H
#define GUARD_WIRELESS_LINK_INDICATOR_H
#include "projected_display.h"
#define gRfuLinkStatus gUnk_03006A80
#include "rfu_compat.h"
struct WirelessLinkIndicator {
    u8 task[0x18];
    u32 frame, animationTick, selectedFrame;
    struct ProjectedDisplayResource resource;
    struct ProjectedDisplayEntry animation;
};
extern struct WirelessLinkIndicator *gUnk_020000F4;
void WirelessLinkIndicator_SelectStrengthFrame(struct WirelessLinkIndicator *, s32);
void WirelessLinkIndicator_UpdateLinkStatus(struct WirelessLinkIndicator *);
s32 WirelessLinkIndicator_Initialize(struct WirelessLinkIndicator *);
s32 WirelessLinkIndicator_Update(struct WirelessLinkIndicator *);
s32 WirelessLinkIndicator_Destroy(struct WirelessLinkIndicator *);
struct WirelessLinkIndicator *WirelessLinkIndicator_Get(void);
struct WirelessLinkIndicator *WirelessLinkIndicator_Create(void);
u32 sub_08036BEC(void);
s32 sub_08036A3C(void);
s32 sub_08036A10(void);
#endif
