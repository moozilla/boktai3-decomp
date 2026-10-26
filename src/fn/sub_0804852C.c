#include "wireless_link_indicator.h"
s32 Widget_InitLinked(void *, void *, u32, u32, u32, u32, u32, void *);
s32 WirelessLinkIndicator_Initialize(struct WirelessLinkIndicator *indicator)
{
    const struct ProjectedDisplayResource *resource;
    struct ProjectedDisplayResource *relocated;
    s32 result;
    gWirelessLinkIndicator = indicator;
    resource = ScriptProc_Start(0xCB05, 0x7C8B);
    if (resource) {
        indicator->resource = *resource;
        relocated = &indicator->resource;
        ResourceHeader_Relocate(relocated, (void *)resource);
        Widget_InitLinked(&indicator->animation, relocated, 34, 17, 0, 0, 60, NULL);
        result = 0;
    } else
        result = -1;
    return result;
}
