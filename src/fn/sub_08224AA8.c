#include "global.h"
#define gRfuSlotStatusNI gUnk_03006A70
#define gRfuLinkStatus   gUnk_03006A80
#include "rfu_compat.h"
struct Manager {
    u8 pad0[0x18];
    u16 f18;
};
extern struct Manager gUnk_03005390;

void RfuManager_DropFailingTargets(void)
{
    u8 i, j, bm;
    if (gUnk_03005390.f18 != 0) {
        if (gRfuLinkStatus->sendSlotNIFlag) {
            for (i = 0; i < 4; i++) {
                if (gRfuSlotStatusNI[i]->send.state & SLOT_BUSY_FLAG) {
                    bm = 0;
                    for (j = 0; j < 4; j++) {
                        if ((gRfuSlotStatusNI[i]->send.bmSlot >> j) & 1) {
                            if (gRfuSlotStatusNI[j]->send.failCounter > gUnk_03005390.f18)
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
                    gRfuSlotStatusNI[i]->recv.failCounter > gUnk_03005390.f18)
                    rfu_NI_stopReceivingData(i);
            }
        }
    }
}
