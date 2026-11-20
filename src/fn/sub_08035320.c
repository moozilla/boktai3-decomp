#include "wireless_controller.h"
#include "rfu_compat.h"
#include "ram.h"
#include "rfu_manager.h"
void Wireless_UpdateUniAcknowledgedSlots(WirelessController *p)
{
    WIRELESS_RECORD_PARENT_ACK(p, gRfuLinkManager.parentAck_flag, p->roster.slots);
}
