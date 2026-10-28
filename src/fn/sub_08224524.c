#include "global.h"
#include "rfu_manager.h"

extern u8 *gRfuLinkStatus;
void RfuManager_UpdateLinkRecovery(u16);
void RfuManager_CompletePendingMscCycle(void);
u16 rfu_UNI_PARENT_getDRAC_ACK(u8 *);
void Callback_InvokeActor(u16, u32);
void rfu_waitREQComplete(void);

void Rfu_ReqCallback(u16 a)
{
    u8 saved = gRfuLinkManager.active;
    u8 zero, ack;
    u32 cb;
    gRfuLinkManager.active = 0;
    gRfuLinkManager.msc_exe_flag = 1;
    zero = *gRfuLinkStatus;
    if (zero == 0) {
        RfuManager_UpdateLinkRecovery(a);
        if (gRfuLinkManager.childClockSlave_flag != 1) {
            RfuManager_CompletePendingMscCycle();
            gRfuLinkManager.msc_exe_flag = zero;
            gRfuLinkManager.active = saved;
            return;
        }
    } else {
        if (rfu_UNI_PARENT_getDRAC_ACK(&ack) == 0) {
            gRfuLinkManager.parentAck_flag |= ack;
        }
    }
    cb = gRfuLinkManager.unk44;
    if (cb != 0) {
        Callback_InvokeActor(a, cb);
        rfu_waitREQComplete();
        if (gRfuLinkManager.childClockSlave_flag == 2) {
            RfuManager_CompletePendingMscCycle();
        }
    }
    gRfuLinkManager.msc_exe_flag = 0;
    gRfuLinkManager.active = saved;
}
