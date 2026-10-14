#define WirelessLinkIndicator_Get sub_080483A0
#define WirelessLinkIndicator_Update sub_080484FC
#define WirelessLinkIndicator_Destroy sub_08048514
#define WirelessLinkIndicator_Initialize sub_0804852C
#include "wireless_link_indicator.h"
#define WirelessLinkIndicator_Create sub_08048598
struct WirelessLinkIndicator *WirelessLinkIndicator_Create(void)
{
    struct WirelessLinkIndicator *p = WirelessLinkIndicator_Get();
    struct WirelessLinkIndicator *result;
    if (p) {
        result = p;
        goto done;
    }
    {
        struct WirelessLinkIndicator *allocated = Task_Create(11, 0xA4);
        void *update;
        void *destroy;
        p = allocated;
        if (!p)
            goto returnPointer;
        update = WirelessLinkIndicator_Update;
        destroy = WirelessLinkIndicator_Destroy;
        Task_SetCallbacks(allocated, update, destroy);
    }
    if (WirelessLinkIndicator_Initialize(p) >= 0)
        goto returnPointer;
    Task_Kill(p);
    result = 0;
    goto done;
returnPointer:
    result = p;
done:
    return result;
}
