#include "global.h"
#include "rfu_compat.h"

void RfuManager_RemoveSendTargets(u8 bm)
{
    u8 i;
    if (gRfuLinkStatus->sendSlotNIFlag) {
        for (i = 0; i < 4; i++) {
            if ((gRfuSlotStatusNI[i]->send.state & SLOT_BUSY_FLAG) &&
                (bm & gRfuSlotStatusNI[i]->send.bmSlot))
                rfu_changeSendTarget(0x20, i, gRfuSlotStatusNI[i]->send.bmSlot & ~bm);
        }
    }
    if (gRfuLinkStatus->recvSlotNIFlag) {
        for (i = 0; i < 4; i++) {
            if ((gRfuSlotStatusNI[i]->recv.state & SLOT_BUSY_FLAG) &&
                (gRfuSlotStatusNI[i]->recv.bmSlot & bm))
                rfu_NI_stopReceivingData(i);
        }
    }
    if (gRfuLinkStatus->sendSlotUNIFlag) {
        gRfuLinkStatus->sendSlotUNIFlag &= ~bm;
        for (i = 0; i < 4; i++) {
            if (gRfuSlotStatusUNI[i]->send.state == SLOT_STATE_SEND_UNI &&
                (bm & gRfuSlotStatusUNI[i]->send.bmSlot))
                gRfuSlotStatusUNI[i]->send.bmSlot &= ~bm;
        }
    }
}
