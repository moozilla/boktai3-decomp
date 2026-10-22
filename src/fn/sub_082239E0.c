#include "global.h"
#define gRfuSlotStatusNI gUnk_03006A70
#define gRfuLinkStatus   gUnk_03006A80
#include "rfu_compat.h"
struct Manager {
    u8 f0, f1;
    volatile u8 f2;
    u8 f3, f4, f5, f6, f7, f8, f9, fa, fb, fc, fd, fe, ff;
    u8 f10, f11, f12, p13;
    u16 f14, f16, f18, f1a, f1c, f1e;
    const u16 *allowed;
    u8 f24, p25;
    u16 f26, a28[4];
    u8 f30, p31;
    u16 f32;
    u16 a34[4];
};
void RfuManager_NotifyEvent(u8, u8);
void RfuManager_DisconnectAndWait(u8);
void RfuManager_CompletePendingMscCycle(void);
void RfuManager_ClearState(void);
void RfuManager_FinishSlotReception(void);
u8 RfuManager_FindAllowedParents(void);
void RfuManager_RemoveSendTargets(void);
extern struct Manager gUnk_03005390;
#define lman gUnk_03005390
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
        lman.f14 = bm_linkLossSlot;
        lman.f16 = reason;
        if (lman.f9) {
            lman.fa = 1;
            if (lman.f6 == 0 && reason == 0)
                lman.fa = 4;
            if (lman.fa == 1) {
                for (i = 0; i < 4; i++) {
                    if ((bm_linkLossSlot >> i) & 1) {
                        lman.f30 |= (1 << i);
                        lman.a34[i] = lman.f32;
                    }
                }
                RfuManager_NotifyEvent(0x31, 1);
            } else {
                lman.fa = 0;
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
                if ((lman.f30 >> i) & 1 && (bm_linkRecoverySlot >> i) & 1)
                    lman.a34[i] = 0;
            }
            lman.f30 &= ~bm_linkRecoverySlot;
            lman.f14 = bm_linkRecoverySlot;
            RfuManager_NotifyEvent(0x32, 1);
        }
        if (lman.f30) {
            bm_disconnectSlot = 0;
            for (i = 0; i < 4; i++) {
                if ((lman.f30 >> i) & 1 && lman.a34[i] && --lman.a34[i] == 0) {
                    lman.f30 &= ~(1 << i);
                    bm_disconnectSlot |= (1 << i);
                }
            }
            if (bm_disconnectSlot) {
                RfuManager_DisconnectAndWait(bm_disconnectSlot);
                disconnect_occure_flag = TRUE;
                lman.f14 = bm_disconnectSlot;
                RfuManager_NotifyEvent(0x33, 1);
            }
        }
        if (!lman.f30)
            lman.fa = 0;
    }
    return disconnect_occure_flag;
}
