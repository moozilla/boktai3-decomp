#include "wireless_controller.h"
#include "rfu_compat.h"
#include "ram.h"
s32 Wireless_EnqueuePacket(void *, void *);
void Wireless_BuildParentUniPacket(WirelessController *p)
{
    struct WirelessPeerRoster *roster = &p->roster;
    struct WirelessParentInputPacket *packet = &p->parentSend;
    if ((p->acknowledgedSlots & roster->slots) == roster->slots) {
        if (p->receivedInput) {
            if (roster->count > 1)
                p->sendReady = 1;
        } else {
            /* "+ 0": one more gcse insn outside i's live range, for PRE's stack-slot order (scope preorder) */
            int sequence = (u8)(packet->sequence + 0);
            int i;
            Wireless_EnqueuePacket(p->packetQueue, packet);
            p->sendReady = 0;
            CpuFill32(0, packet, sizeof(*packet));
            for (i = 0; i <= 3; i++) {
                int bit = 2 << i;
                if (roster->slots & bit)
                    packet->childInput[i] = p->parentReceive[i].input;
            }
            packet->input = p->previousSentInput;
            p->previousSentInput = p->pendingInput;
            p->pendingInput = 0xFFFF;
            packet->slots = roster->slots;
            packet->count = 1;
            i = 0;
            {
                int next = sequence + 1;
                do {
                    if (roster->slots & (2 << i))
                        packet->count++;
                } while (++i <= 3);
                packet->sequence = next;
            }
            p->acknowledgedSlots = 1;
            p->receivedSlots = 1;
            if (roster->count > 1)
                p->sendReady = 1;
        }
    }
}
