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
    u8 f30, p31[3];
    u16 a34[4];
};
void RfuManager_NotifyEvent(u8, u8);
void RfuManager_DisconnectAndWait(u8);
void RfuManager_CompletePendingMscCycle(void);
void RfuManager_ClearState(void);
void RfuManager_FinishSlotReception(void);
u8 RfuManager_FindAllowedParents(void);
void RfuManager_RemoveSendTargets(void);
void RfuManager_UpdateLinkRecovery(u16);
extern struct Manager gUnk_03005390;
#define lman gUnk_03005390
// pret/pokeemerald AgbRfu_LinkManager.c: rfu_LMAN_PARENT_checkRecvChildName
void RfuManager_UpdateChildAdmissions(void)
{
    u8 newSlot;
    u8 newAcceptSlot;
    u8 i;
    u8 flags;
    u8 tgtSlot;
    const u16 *ptr;

    if (lman.f4 == 5 || lman.f4 == 6 || lman.f4 == 7 || lman.f4 == 8) {
        newSlot = ((gRfuLinkStatus->connSlotFlag ^ lman.fc) & gRfuLinkStatus->connSlotFlag) &
                  ~gRfuLinkStatus->getNameFlag;
        lman.fc = gRfuLinkStatus->connSlotFlag;
        if (newSlot) {
            lman.f14 = newSlot;
            RfuManager_NotifyEvent(0x10, 1);
        }
        newAcceptSlot = 0;
        for (i = 0; i < 4; i++) {
            tgtSlot = 1 << i;
            flags = 0;
            if (newSlot & tgtSlot) {
                lman.a28[i] = lman.f26;
                lman.f24 |= tgtSlot;
            } else if (lman.f24 & tgtSlot) {
                if (gRfuSlotStatusNI[i]->recv.state == 0x46) {
                    if (gRfuSlotStatusNI[i]->recv.dataType == 1) {
                        flags = 2;
                        for (ptr = lman.allowed; *ptr != 0xFFFF; ptr++) {
                            if (gRfuLinkStatus->partner[i].serialNo == *ptr) {
                                lman.f0 |= tgtSlot;
                                lman.f1++;
                                newAcceptSlot |= tgtSlot;
                                flags |= 1;
                                break;
                            }
                        }
                        if (!(flags & 1))
                            flags |= 4;
                    }
                } else if (--lman.a28[i] == 0) {
                    flags = 2 | 4;
                }
                if (flags & 2) {
                    lman.f24 &= ~tgtSlot;
                    lman.a28[i] = 0;
                    rfu_clearSlot(8, i);
                }
                if (flags & 4)
                    lman.fd |= tgtSlot;
            }
        }
        if (newAcceptSlot) {
            lman.f14 = newAcceptSlot;
            RfuManager_NotifyEvent(0x11, 1);
        }
        if (lman.fd) {
            flags = 1;
            if (gRfuLinkStatus->sendSlotUNIFlag) {
                if ((lman.f3 & lman.f0) != lman.f0)
                    flags = 0;
            }
            if (flags) {
                RfuManager_DisconnectAndWait(lman.fd);
                lman.f14 = lman.fd;
                lman.fd = 0;
                RfuManager_NotifyEvent(0x12, 1);
            }
        }
        if (lman.f24 == 0 && lman.f4 == 8) {
            if (lman.f7 == 0) {
                lman.f4 = lman.f5 = 0;
                RfuManager_NotifyEvent(0x14, 0);
            } else {
                if (lman.f7 == 2) {
                    lman.f7 = 3;
                    lman.f4 = 9;
                } else {
                    lman.f7 = 1;
                    lman.f4 = 5;
                }
                if (lman.f0) {
                    lman.f1a = 0;
                    lman.f7 = 8;
                    lman.f4 = 5;
                }
            }
        }
    }
}
