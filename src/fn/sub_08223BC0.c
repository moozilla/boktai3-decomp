#include "global.h"
#define gRfuLinkStatus gUnk_03006A80
#include "rfu_compat.h"
struct RfuConfig {
    u8 maxMFrame, mcTimer;
    u16 availSlotFlag;
    u8 mbootFlag, pad5;
    u16 serialNo;
    const u8 *gname;
    const u8 *uname;
};
struct Manager {
    u8 f0, f1;
    volatile u8 f2;
    u8 f3, f4, f5, f6, f7, f8, f9, fa, fb, fc, fd, fe, ff;
    u8 f10, f11, f12, p13;
    u16 f14, f16, f18, f1a, f1c, f1e;
    u8 pad20[0x1C];
    struct RfuConfig *config;
    u32 f40;
};
extern struct Manager gUnk_03005390;
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
    if (gUnk_03005390.f40 == 0 && gUnk_03005390.f4 != 0) {
        gUnk_03005390.f4 = 0;
        return;
    }
    if (gUnk_03005390.f7)
        RfuManager_StepSlotTimer(arg);
    do {
        if (gUnk_03005390.f4) {
            rfu_waitREQComplete();
            gUnk_03005390.fe = 1;
            switch (gUnk_03005390.f4) {
                case 23: {
                    u32 result = RfuManager_ResetAndCheckAdapter();
                    u8 event = result == 0x8001 ? 0x44 : 0xFF;
                    gUnk_03005390.f5 = 0;
                    gUnk_03005390.f4 = 0;
                    RfuManager_NotifyEvent(event, 0);
                    break;
                }
                case 1:
                    if (RfuManager_ResetAndCheckAdapter() == 0x8001) {
                        gUnk_03005390.f4 = gUnk_03005390.f5;
                        gUnk_03005390.f5 = 3;
                    } else {
                        gUnk_03005390.f5 = 0;
                        gUnk_03005390.f4 = 0;
                        RfuManager_NotifyEvent(0xFF, 0);
                    }
                    break;
                case 2:
                    rfu_REQ_reset();
                    break;
                case 3:
                    rfu_REQ_configSystem(gUnk_03005390.config->availSlotFlag,
                                         gUnk_03005390.config->maxMFrame,
                                         gUnk_03005390.config->mcTimer);
                    break;
                case 4:
                    rfu_REQ_configGameData(
                        gUnk_03005390.config->mbootFlag, gUnk_03005390.config->serialNo,
                        gUnk_03005390.config->gname, gUnk_03005390.config->uname);
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
                    rfu_REQ_startConnectParent(gUnk_03005390.f1e);
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
            gUnk_03005390.fe = 0;
        }
    } while ((u8)(gUnk_03005390.f4 - 0x12) <= 1);
    if (gRfuLinkStatus->parentChild == 1 && RfuManager_UpdateLinkRecovery(0))
        return;
    RfuManager_UpdateChildAdmissions();
    sub_082247CC();
    RfuManager_StartPendingRecovery();
    RfuManager_DropFailingTargets();
}
