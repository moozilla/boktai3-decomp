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
#define lman gRfuLinkManager
// pret/pokeemerald AgbRfu_LinkManager.c: rfu_LMAN_linkWatcher
u8 RfuManager_UpdateLinkRecovery(u16 REQ_commandID)
{
    u8 i;
    u8 bm_linkLossSlot;
    u8 reason;
    u8 bm_linkRecoverySlot;
    u8 bm_disconnectSlot;

    u8 disconnect_occure_flag = FALSE;
    rfu_REQBN_watchLink(REQ_commandID, &bm_linkLossSlot, &reason, &bm_linkRecoverySlot);
    if (bm_linkLossSlot) {
        lman.event.words.code = bm_linkLossSlot;
        lman.event.words.value = reason;
        if (lman.linkRecovery_enable) {
            lman.linkRecovery_start_flag = 1;
            if (lman.parent_child == 0 && reason == 0)
                lman.linkRecovery_start_flag = 4;
            if (lman.linkRecovery_start_flag == 1) {
                for (i = 0; i < 4; i++) {
                    if ((bm_linkLossSlot >> i) & 1) {
                        lman.linkRecoveryTimer.active |= (1 << i);
                        lman.linkRecoveryTimer.count[i] = lman.linkRecoveryTimer.count_max;
                    }
                }
                RfuManager_NotifyEvent(0x31, 1);
            } else {
                lman.linkRecovery_start_flag = 0;
                RfuManager_DisconnectAndWait(bm_linkLossSlot);
                disconnect_occure_flag = TRUE;
                RfuManager_NotifyEvent(0x33, 1);
            }
        } else {
            RfuManager_DisconnectAndWait(bm_linkLossSlot);
            disconnect_occure_flag = TRUE;
            RfuManager_NotifyEvent(0x30, 2);
        }
        RfuManager_CompletePendingMscCycle();
    }
    if (gRfuLinkStatus->parentChild == 1) {
        if (bm_linkRecoverySlot) {
            for (i = 0; i < 4; i++) {
                if ((lman.linkRecoveryTimer.active >> i) & 1 && (bm_linkRecoverySlot >> i) & 1)
                    lman.linkRecoveryTimer.count[i] = 0;
            }
            lman.linkRecoveryTimer.active &= ~bm_linkRecoverySlot;
            lman.event.words.code = bm_linkRecoverySlot;
            RfuManager_NotifyEvent(0x32, 1);
        }
        if (lman.linkRecoveryTimer.active) {
            bm_disconnectSlot = 0;
            for (i = 0; i < 4; i++) {
                if ((lman.linkRecoveryTimer.active >> i) & 1 && lman.linkRecoveryTimer.count[i] && --lman.linkRecoveryTimer.count[i] == 0) {
                    lman.linkRecoveryTimer.active &= ~(1 << i);
                    bm_disconnectSlot |= (1 << i);
                }
            }
            if (bm_disconnectSlot) {
                RfuManager_DisconnectAndWait(bm_disconnectSlot);
                disconnect_occure_flag = TRUE;
                lman.event.words.code = bm_disconnectSlot;
                RfuManager_NotifyEvent(0x33, 1);
            }
        }
        if (!lman.linkRecoveryTimer.active)
            lman.linkRecovery_start_flag = 0;
    }
    return disconnect_occure_flag;
}
