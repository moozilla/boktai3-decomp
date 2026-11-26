#include "engine/shared/wireless_controller.h"
#include "lib/rfu/rfu_compat.h"
#include "engine/shared/ram.h"
#include "link/rfu_manager/rfu_manager.h"
void Wireless_UpdateUniAcknowledgedSlots(WirelessController *p)
{
    WIRELESS_RECORD_PARENT_ACK(p, gRfuLinkManager.parentAck_flag, p->roster.slots);
}
