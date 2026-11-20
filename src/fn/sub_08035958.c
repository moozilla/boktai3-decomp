#include "wireless_controller.h"
#include "ram.h"
#include "rfu_compat.h"

void Wireless_OnParentUniSendComplete(void)
{
    WirelessController *p = gWirelessController;
    u8 slots;
    if (p) {
        struct WirelessPeerRoster *roster = &p->roster;
        int status = rfu_UNI_PARENT_getDRAC_ACK(&slots);
        if (!status) {
            WIRELESS_RECORD_PARENT_ACK(p, slots, roster->slots);
        }
        rfu_REQ_recvData();
    }
}
