#include "link/wireless/wireless_controller.h"
#include "lib/rfu/rfu_compat.h"
#include "engine/ram.h"
#include "link/wireless/rfu_manager.h"
void Wireless_UpdateUniAcknowledgedSlots(WirelessController *p)
{
    WIRELESS_RECORD_PARENT_ACK(p, gRfuLinkManager.parentAck_flag, p->roster.slots);
}
