#include "global.h"
#define gRfuSlotStatusNI gUnk_03006A70
#define gRfuLinkStatus   gUnk_03006A80
#include "rfu_compat.h"
#include "rfu_manager.h"
void RfuManager_NotifyEvent(u8, u8);
void RfuManager_DisconnectAndWait(u8);
void RfuManager_CompletePendingMscCycle(void);
void RfuManager_ClearState(void);
void RfuManager_FinishSlotReception(void);
u8 RfuManager_FindAllowedParents(void);
void RfuManager_RemoveSendTargets(void);
void RfuManager_UpdateLinkRecovery(u16);
#define lman gRfuLinkManager
// pret/pokeemerald AgbRfu_LinkManager.c: rfu_LMAN_PARENT_checkRecvChildName
void RfuManager_UpdateChildAdmissions(void)
{
    u8 newSlot;
    u8 newAcceptSlot;
    u8 i;
    u8 flags;
    u8 tgtSlot;
    const u16 *ptr;

    if (lman.state == 5 || lman.state == 6 || lman.state == 7 || lman.state == 8) {
        newSlot = ((gRfuLinkStatus->connSlotFlag ^ lman.connectSlot_flag_old) & gRfuLinkStatus->connSlotFlag) &
                  ~gRfuLinkStatus->getNameFlag;
        lman.connectSlot_flag_old = gRfuLinkStatus->connSlotFlag;
        if (newSlot) {
            lman.event.words.code = newSlot;
            RfuManager_NotifyEvent(0x10, 1);
        }
        newAcceptSlot = 0;
        for (i = 0; i < 4; i++) {
            tgtSlot = 1 << i;
            flags = 0;
            if (newSlot & tgtSlot) {
                lman.nameAcceptTimer.count[i] = lman.nameAcceptTimer.count_max;
                lman.nameAcceptTimer.active |= tgtSlot;
            } else if (lman.nameAcceptTimer.active & tgtSlot) {
                if (gRfuSlotStatusNI[i]->recv.state == 0x46) {
                    if (gRfuSlotStatusNI[i]->recv.dataType == 1) {
                        flags = 2;
                        for (ptr = lman.acceptable_serialNo_list; *ptr != 0xFFFF; ptr++) {
                            if (gRfuLinkStatus->partner[i].serialNo == *ptr) {
                                lman.acceptSlot_flag |= tgtSlot;
                                lman.acceptCount++;
                                newAcceptSlot |= tgtSlot;
                                flags |= 1;
                                break;
                            }
                        }
                        if (!(flags & 1))
                            flags |= 4;
                    }
                } else if (--lman.nameAcceptTimer.count[i] == 0) {
                    flags = 2 | 4;
                }
                if (flags & 2) {
                    lman.nameAcceptTimer.active &= ~tgtSlot;
                    lman.nameAcceptTimer.count[i] = 0;
                    rfu_clearSlot(8, i);
                }
                if (flags & 4)
                    lman.reserveDisconnectSlot_flag |= tgtSlot;
            }
        }
        if (newAcceptSlot) {
            lman.event.words.code = newAcceptSlot;
            RfuManager_NotifyEvent(0x11, 1);
        }
        if (lman.reserveDisconnectSlot_flag) {
            flags = 1;
            if (gRfuLinkStatus->sendSlotUNIFlag) {
                if ((lman.parentAck_flag & lman.acceptSlot_flag) != lman.acceptSlot_flag)
                    flags = 0;
            }
            if (flags) {
                RfuManager_DisconnectAndWait(lman.reserveDisconnectSlot_flag);
                lman.event.words.code = lman.reserveDisconnectSlot_flag;
                lman.reserveDisconnectSlot_flag = 0;
                RfuManager_NotifyEvent(0x12, 1);
            }
        }
        if (lman.nameAcceptTimer.active == 0 && lman.state == 8) {
            if (lman.pcswitch_flag == 0) {
                lman.state = lman.next_state = 0;
                RfuManager_NotifyEvent(0x14, 0);
            } else {
                if (lman.pcswitch_flag == 2) {
                    lman.pcswitch_flag = 3;
                    lman.state = 9;
                } else {
                    lman.pcswitch_flag = 1;
                    lman.state = 5;
                }
                if (lman.acceptSlot_flag) {
                    lman.connect_period = 0;
                    lman.pcswitch_flag = 8;
                    lman.state = 5;
                }
            }
        }
    }
}
