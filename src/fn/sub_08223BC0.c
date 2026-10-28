#include "global.h"
#include "rfu_compat.h"
#include "rfu_manager.h"
struct RfuConfig {
    u8 maxMFrame, mcTimer;
    u16 availSlotFlag;
    u8 mbootFlag, pad5;
    u16 serialNo;
    const u8 *gname;
    const u8 *uname;
};
void RfuManager_NotifyEvent(u8, u8);
void RfuManager_StepSlotTimer(u32);
u32 RfuManager_ResetAndCheckAdapter(void);
u8 RfuManager_UpdateLinkRecovery(u16);
void RfuManager_UpdateChildAdmissions(void);
void sub_082247CC(void);
void RfuManager_StartPendingRecovery(void);
void RfuManager_DropFailingTargets(void);

/* Per-frame RFU manager update: issues the request for the current state,
   then services link recovery and the child slots. */
void sub_08223BC0(u32 arg)
{
    if (gRfuLinkManager.unk40 == 0 && gRfuLinkManager.state != 0) {
        gRfuLinkManager.state = 0;
        return;
    }
    if (gRfuLinkManager.pcswitch_flag)
        RfuManager_StepSlotTimer(arg);
    do {
        if (gRfuLinkManager.state) {
            rfu_waitREQComplete();
            gRfuLinkManager.active = 1;
            switch (gRfuLinkManager.state) {
                case 23: {
                    u32 result = RfuManager_ResetAndCheckAdapter();
                    u8 event = result == 0x8001 ? 0x44 : 0xFF;
                    gRfuLinkManager.next_state = 0;
                    gRfuLinkManager.state = 0;
                    RfuManager_NotifyEvent(event, 0);
                    break;
                }
                case 1:
                    if (RfuManager_ResetAndCheckAdapter() == 0x8001) {
                        gRfuLinkManager.state = gRfuLinkManager.next_state;
                        gRfuLinkManager.next_state = 3;
                    } else {
                        gRfuLinkManager.next_state = 0;
                        gRfuLinkManager.state = 0;
                        RfuManager_NotifyEvent(0xFF, 0);
                    }
                    break;
                case 2:
                    rfu_REQ_reset();
                    break;
                case 3:
                    rfu_REQ_configSystem(gRfuLinkManager.config->availSlotFlag,
                                         gRfuLinkManager.config->maxMFrame,
                                         gRfuLinkManager.config->mcTimer);
                    break;
                case 4:
                    rfu_REQ_configGameData(
                        gRfuLinkManager.config->mbootFlag, gRfuLinkManager.config->serialNo,
                        gRfuLinkManager.config->gname, gRfuLinkManager.config->uname);
                    break;
                case 5:
                    rfu_REQ_startSearchChild();
                    break;
                case 6:
                    rfu_REQ_pollSearchChild();
                    break;
                case 7:
                    rfu_REQ_endSearchChild();
                    break;
                case 9:
                    rfu_REQ_startSearchParent();
                    break;
                case 10:
                    rfu_REQ_pollSearchParent();
                    break;
                case 11:
                    rfu_REQ_endSearchParent();
                    break;
                case 12:
                    rfu_REQ_startConnectParent(gRfuLinkManager.work);
                    break;
                case 13:
                    rfu_REQ_pollConnectParent();
                    break;
                case 14:
                    rfu_REQ_endConnectParent();
                    break;
                case 16:
                    rfu_REQ_CHILD_startConnectRecovery(gRfuLinkStatus->linkLossSlotFlag);
                    break;
                case 17:
                    rfu_REQ_CHILD_pollConnectRecovery();
                    break;
                case 18:
                    rfu_REQ_CHILD_endConnectRecovery();
                    break;
                case 19:
                    rfu_REQ_changeMasterSlave();
                    break;
                case 21:
                    rfu_REQ_stopMode();
                    break;
            }
            rfu_waitREQComplete();
            gRfuLinkManager.active = 0;
        }
    } while ((u8)(gRfuLinkManager.state - 0x12) <= 1);
    if (gRfuLinkStatus->parentChild == 1 && RfuManager_UpdateLinkRecovery(0))
        return;
    RfuManager_UpdateChildAdmissions();
    sub_082247CC();
    RfuManager_StartPendingRecovery();
    RfuManager_DropFailingTargets();
}
