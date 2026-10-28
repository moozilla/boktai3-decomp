#include "global.h"
#define gRfuLinkStatus gUnk_03006A80
#include "rfu_compat.h"
#include "rfu_manager.h"
#define LMAN_MSG_INITIALIZE_COMPLETED                    0x00
#define LMAN_MSG_SEARCH_CHILD_PERIOD_EXPIRED             0x13
#define LMAN_MSG_PARENT_FOUND                            0x20
#define LMAN_MSG_SEARCH_PARENT_PERIOD_EXPIRED            0x21
#define LMAN_MSG_CHILD_NAME_SEND_FAILED_AND_DISCONNECTED 0x25
#define LMAN_MSG_LINK_DISCONNECTED_BY_USER               0x40
#define LMAN_MSG_CHANGE_AGB_CLOCK_SLAVE                  0x41
#define LMAN_MSG_RFU_POWER_DOWN                          0x42
#define LMAN_MSG_RECV_DATA_REQ_COMPLETED                 0x50
#define LMAN_MSG_REQ_API_ERROR                           0xf0
#define LMAN_MSG_CLOCK_SLAVE_MS_CHANGE_ERROR_BY_DMA      0xf2
#define RFU_CHILD_CLOCK_SLAVE_ON                         1
#define FSP_ON                                           1
#define PCSWITCH_2ND_SP_START                            0x03
#define PCSWITCH_2ND_SP                                  0x04
#define PCSWITCH_3RD_SC_START                            0x05
#define PCSWITCH_3RD_SC                                  0x06
#define PCSWITCH_CP                                      0x07
#define PCSWITCH_SC_LOCK                                 0x08
#define PCSWITCH_1ST_SC_START                            0x01
#define LMAN_STATE_READY                                 0x00
#define LMAN_STATE_CONFIG_GAME_DATA                      0x04
#define LMAN_STATE_START_SEARCH_CHILD                    0x05
#define LMAN_STATE_POLL_SEARCH_CHILD                     0x06
#define LMAN_STATE_END_SEARCH_CHILD                      0x07
#define LMAN_STATE_WAIT_RECV_CHILD_NAME                  0x08
#define LMAN_STATE_START_SEARCH_PARENT                   0x09
#define LMAN_STATE_POLL_SEARCH_PARENT                    0x0a
#define LMAN_STATE_END_SEARCH_PARENT                     0x0b
#define LMAN_STATE_POLL_CONNECT_PARENT                   0x0d
#define LMAN_STATE_END_CONNECT_PARENT                    0x0e
#define LMAN_STATE_SEND_CHILD_NAME                       0x0f
#define LMAN_STATE_POLL_LINK_RECOVERY                    0x11
#define LMAN_STATE_END_LINK_RECOVERY                     0x12
#define LMAN_STATE_MS_CHANGE                             0x13
#define LMAN_STATE_BACK_STATE                            0x16


/* pret/pokeemerald AgbRfu_LinkManager.h: struct RfuLinkManager (gRfuLinkManager). */
#define lman gRfuLinkManager
void RfuManager_NotifyEvent(u8, u8);
void RfuManager_DisconnectAndWait(u8);
void RfuManager_CompletePendingMscCycle(void);
void RfuManager_ClearState(void);
void RfuManager_FinishSlotReception(void);
u8 RfuManager_FindAllowedParents(void);
void RfuManager_RemoveSendTargets(u8);
u8 RfuManager_UpdateLinkRecovery(u16);
// pret/pokeemerald AgbRfu_LinkManager.c: rfu_LMAN_REQ_callback
void RfuManager_ApplyRequestResult(u16 reqCommandId, u16 reqResult)
{
    u8 status;
    u8 *stwiRecvBuffer;
    u8 i;

    if (lman.active != 0) {
        lman.active = 0;
        switch (reqCommandId) {
            case ID_RESET_REQ:
                if (reqResult == 0) {
                    lman.state = lman.next_state;
                    lman.next_state = LMAN_STATE_CONFIG_GAME_DATA;
                }
                break;
            case ID_SYSTEM_CONFIG_REQ:
                if (reqResult == 0) {
                    lman.state = lman.next_state;
                    lman.next_state = LMAN_STATE_READY;
                }
                break;
            case ID_GAME_CONFIG_REQ:
                if (reqResult == 0) {
                    lman.state = lman.next_state = LMAN_STATE_READY;
                    RfuManager_NotifyEvent(LMAN_MSG_INITIALIZE_COMPLETED, 0);
                }
                break;
            case ID_SC_START_REQ:
                if (reqResult == 0) {
                    lman.state = lman.next_state = LMAN_STATE_POLL_SEARCH_CHILD;
                }
                break;
            case ID_SC_POLL_REQ:
                if (lman.connect_period && --lman.connect_period == 0) {
                    lman.state = LMAN_STATE_END_SEARCH_CHILD;
                    lman.next_state = LMAN_STATE_WAIT_RECV_CHILD_NAME;
                }
                break;
            case ID_SC_END_REQ:
                if (reqResult == 0) {
                    lman.state = lman.next_state;
                    lman.next_state = LMAN_STATE_READY;
                    if (lman.pcswitch_flag == 0) {
                        RfuManager_NotifyEvent(LMAN_MSG_SEARCH_CHILD_PERIOD_EXPIRED, 0);
                    }
                }
                break;
            case ID_SP_START_REQ:
                if (reqResult == 0) {
                    if (lman.fastSearchParent_flag == FSP_ON) {
                        if (lman.connect_period > 1) {
                            lman.connect_period--;
                        }
                    }
                    lman.state = lman.next_state = LMAN_STATE_POLL_SEARCH_PARENT;
                }
                break;
            case ID_SP_POLL_REQ:
                if (reqResult == 0) {
                    status = RfuManager_FindAllowedParents();
                    lman.event.param[0] = status;
                    if (status) {
                        RfuManager_NotifyEvent(LMAN_MSG_PARENT_FOUND, 1);
                    }
                    if (lman.fastSearchParent_flag && lman.connect_period != 1 &&
                        gRfuLinkStatus->findParentCount == RFU_CHILD_MAX) {
                        rfu_REQ_endSearchParent();
                        rfu_waitREQComplete();
                        lman.state = LMAN_STATE_START_SEARCH_PARENT;
                        lman.fastSearchParent_flag = FSP_ON;
                    }
                }
                if (lman.connect_period && --lman.connect_period == 0) {
                    lman.state = LMAN_STATE_END_SEARCH_PARENT;
                    lman.next_state = LMAN_STATE_READY;
                }
                break;
            case ID_SP_END_REQ:
                if (reqResult == 0) {
                    lman.state = lman.next_state;
                    if (lman.pcswitch_flag == 0) {
                        if (lman.state == LMAN_STATE_READY) {
                            RfuManager_NotifyEvent(LMAN_MSG_SEARCH_PARENT_PERIOD_EXPIRED, 0);
                        }
                    } else if (lman.pcswitch_flag != PCSWITCH_CP) {
                        lman.state = LMAN_STATE_START_SEARCH_CHILD;
                        lman.pcswitch_flag = PCSWITCH_3RD_SC_START;
                    }
                }
                break;
            case ID_CP_START_REQ:
                if (reqResult == 0) {
                    lman.state = lman.next_state = LMAN_STATE_POLL_CONNECT_PARENT;
                }
                break;
            case ID_CP_POLL_REQ:
                if (reqResult == 0 && !rfu_getConnectParentStatus(&status, &lman.child_slot) &&
                    !status) {
                    lman.state = LMAN_STATE_END_CONNECT_PARENT;
                }
                if (lman.connect_period && --lman.connect_period == 0) {
                    lman.state = LMAN_STATE_END_CONNECT_PARENT;
                }
                break;
            case ID_CP_END_REQ:
                if (reqResult == 0 && !rfu_getConnectParentStatus(&status, &lman.child_slot)) {
                    if (!status) {
                        lman.state = LMAN_STATE_MS_CHANGE;
                        lman.next_state = LMAN_STATE_SEND_CHILD_NAME;
                        lman.work = 0x22;
                        lman.event.param[0] = lman.child_slot;
                    } else {
                        lman.state = lman.next_state = LMAN_STATE_READY;
                        lman.work = 0x23;
                        lman.event.param[0] = status;
                        if (lman.pcswitch_flag) {
                            lman.pcswitch_flag = PCSWITCH_2ND_SP_START;
                            lman.state = LMAN_STATE_START_SEARCH_PARENT;
                        }
                    }
                    RfuManager_NotifyEvent(lman.work, 0x01);
                    lman.work = 0;
                }
                break;
            case ID_CPR_START_REQ:
                if (reqResult == 0) {
                    lman.event.param[0] = gRfuLinkStatus->linkLossSlotFlag;
                    lman.state = lman.next_state = LMAN_STATE_POLL_LINK_RECOVERY;
                    for (lman.child_slot = 0; lman.child_slot < RFU_CHILD_MAX; lman.child_slot++) {
                        if ((gRfuLinkStatus->linkLossSlotFlag >> lman.child_slot) & 1) {
                            break;
                        }
                    }
                }
                break;
            case ID_CPR_POLL_REQ:
                if (reqResult == 0 && !rfu_CHILD_getConnectRecoveryStatus(&status) && status < 2) {
                    lman.state = LMAN_STATE_END_LINK_RECOVERY;
                }
                if (lman.linkRecoveryTimer.count[lman.child_slot] &&
                    --lman.linkRecoveryTimer.count[lman.child_slot] == 0) {
                    lman.state = LMAN_STATE_END_LINK_RECOVERY;
                }
                break;
            case ID_CPR_END_REQ:
                if (reqResult == 0 && !rfu_CHILD_getConnectRecoveryStatus(&status)) {
                    if (!status) {
                        lman.state = LMAN_STATE_MS_CHANGE;
                        lman.next_state = LMAN_STATE_BACK_STATE;
                        lman.work = 0x32;
                    } else {
                        lman.state = lman.next_state = LMAN_STATE_READY;
                        RfuManager_DisconnectAndWait(gRfuLinkStatus->linkLossSlotFlag);
                        lman.work = 0x33;
                    }
                    lman.linkRecoveryTimer.count[lman.child_slot] = 0;
                    lman.linkRecoveryTimer.active = 0;
                    lman.linkRecovery_start_flag = 0;
                    RfuManager_NotifyEvent(lman.work, 0x01);
                    lman.work = 0;
                }
                break;
            case ID_MS_CHANGE_REQ:
                if (reqResult == 0) {
                    if (lman.next_state == LMAN_STATE_BACK_STATE) {
                        lman.state = lman.state_bak[0];
                        lman.next_state = lman.state_bak[1];
                        lman.childClockSlave_flag = RFU_CHILD_CLOCK_SLAVE_ON;
                        RfuManager_NotifyEvent(LMAN_MSG_CHANGE_AGB_CLOCK_SLAVE, 0);
                    } else if (lman.next_state == LMAN_STATE_SEND_CHILD_NAME) {
                        lman.state = lman.next_state;
                        lman.childClockSlave_flag = RFU_CHILD_CLOCK_SLAVE_ON;
                        RfuManager_NotifyEvent(LMAN_MSG_CHANGE_AGB_CLOCK_SLAVE, 0);
                        lman.nameAcceptTimer.active |= 1 << lman.child_slot;
                        lman.nameAcceptTimer.count[lman.child_slot] =
                            lman.nameAcceptTimer.count_max;
                        rfu_clearSlot(TYPE_NI_SEND, lman.child_slot);
                        status = rfu_NI_CHILD_setSendGameName(lman.child_slot, 0x0e);
                        if (status) {
                            lman.state = lman.next_state = LMAN_STATE_READY;
                            RfuManager_CompletePendingMscCycle();
                            RfuManager_DisconnectAndWait(gRfuLinkStatus->connSlotFlag |
                                                         gRfuLinkStatus->linkLossSlotFlag);
                            lman.event.param[0] = status;
                            RfuManager_NotifyEvent(LMAN_MSG_CHILD_NAME_SEND_FAILED_AND_DISCONNECTED,
                                                   1);
                        }
                    }
                }
                break;
            case ID_STOP_MODE_REQ:
                if (reqResult == 0) {
                    lman.state = lman.next_state = LMAN_STATE_READY;
                    RfuManager_NotifyEvent(LMAN_MSG_RFU_POWER_DOWN, 0);
                }
                break;
        }
        lman.active = 1;
    } else if (reqResult == 3 && lman.msc_exe_flag &&
               (reqCommandId == ID_DATA_TX_REQ || reqCommandId == ID_DATA_RX_REQ ||
                reqCommandId == ID_MS_CHANGE_REQ)) {
        rfu_REQ_RFUStatus();
        rfu_waitREQComplete();
        rfu_getRFUStatus(&status);
        if (status == 0 && gRfuLinkStatus->parentChild == MODE_CHILD) {
            stwiRecvBuffer = rfu_getSTWIRecvBuffer() + 4;
            *stwiRecvBuffer++ = gRfuLinkStatus->connSlotFlag;
            *stwiRecvBuffer = REASON_LINK_LOSS;
            RfuManager_UpdateLinkRecovery(ID_DISCONNECTED_AND_CHANGE_REQ);
            reqResult = 0;
        }
    }
    switch (reqCommandId) {
        case ID_DISCONNECT_REQ:
            if (reqResult == 0) {
                lman.event.param[0] = *(rfu_getSTWIRecvBuffer() + 8);
                RfuManager_RemoveSendTargets(lman.event.param[0]);
                if (lman.linkRecoveryTimer.active) {
                    lman.linkRecoveryTimer.active &= ~lman.event.param[0];
                    for (i = 0; i < RFU_CHILD_MAX; i++) {
                        if ((lman.event.param[0] >> i) & 1) {
                            lman.linkRecoveryTimer.count[i] = 0;
                        }
                    }
                    if (lman.parent_child == MODE_CHILD) {
                        lman.state = lman.next_state = LMAN_STATE_READY;
                    }
                }
                status = lman.acceptSlot_flag & lman.event.param[0];
                for (i = 0; i < RFU_CHILD_MAX; i++) {
                    if ((status >> i) & 1 && lman.acceptCount) {
                        lman.acceptCount--;
                    }
                }
                lman.acceptSlot_flag &= ~lman.event.param[0];
                if (lman.pcswitch_flag) {
                    if (gRfuLinkStatus->parentChild == MODE_NEUTRAL) {
                        if (lman.pcswitch_flag == PCSWITCH_SC_LOCK) {
                            lman.connect_period = lman.pcswitch_period_bak;
                            lman.pcswitch_flag = PCSWITCH_3RD_SC;
                            lman.state = LMAN_STATE_POLL_SEARCH_CHILD;
                        } else if (lman.state != LMAN_STATE_POLL_SEARCH_CHILD &&
                                   lman.state != LMAN_STATE_END_SEARCH_CHILD) {
                            lman.pcswitch_flag = PCSWITCH_1ST_SC_START;
                            lman.state = LMAN_STATE_START_SEARCH_CHILD;
                        }
                    }
                }
                if (gRfuLinkStatus->parentChild == MODE_NEUTRAL) {
                    if (lman.state == LMAN_STATE_READY) {
                        lman.parent_child = MODE_NEUTRAL;
                    }
                }
                if (lman.active == 0) {
                    RfuManager_NotifyEvent(LMAN_MSG_LINK_DISCONNECTED_BY_USER, 1);
                }
            }
            break;
        case ID_DATA_RX_REQ:
            RfuManager_FinishSlotReception();
            if (gRfuLinkStatus->parentChild != MODE_NEUTRAL) {
                RfuManager_NotifyEvent(LMAN_MSG_RECV_DATA_REQ_COMPLETED, 0);
            }
            break;
        case ID_RESET_REQ:
        case ID_STOP_MODE_REQ:
            if (reqResult == 0) {
                lman.reserveDisconnectSlot_flag = 0;
                lman.acceptCount = 0;
                lman.acceptSlot_flag = 0;
                lman.parent_child = MODE_NEUTRAL;
                RfuManager_CompletePendingMscCycle();
                if (reqCommandId == ID_STOP_MODE_REQ) {
                    RfuManager_ClearState();
                }
            }
            break;
    }
    if (reqResult != 0) {
        if (reqCommandId == ID_SP_START_REQ && reqResult != 0 &&
            lman.pcswitch_flag == PCSWITCH_2ND_SP) {
            gRfuLinkStatus->parentChild = MODE_PARENT;
            gRfuLinkStatus->connSlotFlag = 0xF;
            RfuManager_DisconnectAndWait(15);
            rfu_waitREQComplete();
            return;
        } else {
            lman.event.param[0] = reqCommandId;
            lman.event.param[1] = reqResult;
            if (lman.active) {
                lman.state = lman.next_state = LMAN_STATE_READY;
            }
            RfuManager_NotifyEvent(LMAN_MSG_REQ_API_ERROR, 2);
            RfuManager_CompletePendingMscCycle();
        }
    }
    if (reqCommandId == ID_CLOCK_SLAVE_MS_CHANGE_ERROR_BY_DMA_REQ) {
        RfuManager_NotifyEvent(LMAN_MSG_CLOCK_SLAVE_MS_CHANGE_ERROR_BY_DMA, 0);
        RfuManager_CompletePendingMscCycle();
    }
}
