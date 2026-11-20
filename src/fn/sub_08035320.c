#include "wireless_controller.h"
#include "rfu_compat.h"
#include "ram.h"
#include "rfu_manager.h"
void Wireless_UpdateUniAcknowledgedSlots(WirelessController *p)
{
    u8 *seen = &p->acknowledgedSlots;
    int bits = (gRfuLinkManager.parentAck_flag << 1) | *seen;
    int expected;
    p->acknowledgedSlots = bits;
    expected = p->roster.slots;
    if ((bits & expected) == expected) {
        p->waitingForAck = 0;
        p->sendReady = 0;
    } else {
        p->waitingForAck = 1;
        p->ackWaitFrames++;
    }
}
