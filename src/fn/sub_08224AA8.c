#include "global.h"
#include "rfu_compat.h"
#include "rfu_manager.h"

void RfuManager_DropFailingTargets(void)
{
    u8 i, j, bm;
    if (gRfuLinkManager.NI_failCounter_limit != 0) {
        if (gRfuLinkStatus->sendSlotNIFlag) {
            for (i = 0; i < 4; i++) {
                if (gRfuSlotStatusNI[i]->send.state & SLOT_BUSY_FLAG) {
                    bm = 0;
                    for (j = 0; j < 4; j++) {
                        if ((gRfuSlotStatusNI[i]->send.bmSlot >> j) & 1) {
                            if (gRfuSlotStatusNI[j]->send.failCounter > gRfuLinkManager.NI_failCounter_limit)
                                bm |= 1 << j;
                        }
                        if (bm != 0)
                            rfu_changeSendTarget(0x20, i, bm ^ gRfuSlotStatusNI[i]->send.bmSlot);
                    }
                }
            }
        }
        if (gRfuLinkStatus->recvSlotNIFlag) {
            for (i = 0; i < 4; i++) {
                if ((gRfuSlotStatusNI[i]->recv.state & SLOT_BUSY_FLAG) &&
                    gRfuSlotStatusNI[i]->recv.failCounter > gRfuLinkManager.NI_failCounter_limit)
                    rfu_NI_stopReceivingData(i);
            }
        }
    }
}
