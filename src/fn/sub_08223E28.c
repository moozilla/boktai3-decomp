#include "global.h"
#define gRfuLinkStatus gUnk_03006A80
#include "rfu_compat.h"
#define G ((struct Manager *)0x03005390)
struct Manager {
    u8 f0, f1;
    volatile u8 f2;
    u8 f3, f4, f5, f6, f7, f8, f9, fa, fb, fc, fd, fe, ff;
    u8 f10, f11, f12, p13;
    u16 f14, f16, f18, f1a, f1c, f1e;
    u8 p20[4], f24, p25;
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
#ifdef NONMATCHING
// fuzzy: 80.9%
void RfuManager_ApplyRequestResult(u16 command, u16 result)
{
    struct Manager *s = (void *)0x03005390;
    u8 status;
    u8 i;
    if (G->fe) {
        G->fe = 0;
        switch (command) {
            case 0x10:
                if (!result) {
                    G->f4 = G->f5;
                    G->f5 = 4;
                }
                break;
            case 0x17:
                if (!result) {
                    G->f4 = G->f5;
                    G->f5 = 0;
                }
                break;
            case 0x16:
                if (!result) {
                    G->f5 = 0;
                    G->f4 = 0;
                    RfuManager_NotifyEvent(0, 0);
                }
                break;
            case 0x19:
                if (!result) {
                    G->f5 = 6;
                    G->f4 = 6;
                }
                break;
            case 0x1a:
                if (G->f1a && --G->f1a == 0) {
                    G->f4 = 7;
                    G->f5 = 8;
                }
                break;
            case 0x1b:
                if (!result) {
                    G->f4 = G->f5;
                    G->f5 = 0;
                    if (!G->f7)
                        RfuManager_NotifyEvent(0x13, 0);
                }
                break;
            case 0x1c:
                if (!result) {
                    if (G->fb == 1 && G->f1a > 1)
                        --G->f1a;
                    G->f5 = 10;
                    G->f4 = 10;
                }
                break;
            case 0x1d:
                if (!result) {
                    status = RfuManager_FindAllowedParents();
                    G->f14 = status;
                    if (status)
                        RfuManager_NotifyEvent(0x20, 1);
                    if (G->fb && G->f1a != 1 && gRfuLinkStatus->findParentCount == 4) {
                        rfu_REQ_endSearchParent();
                        rfu_waitREQComplete();
                        G->f4 = 9;
                        G->fb = 1;
                    }
                }
                if (G->f1a && --G->f1a == 0) {
                    G->f4 = 11;
                    G->f5 = 0;
                }
                break;
            case 0x1e:
                if (!result) {
                    G->f4 = G->f5;
                    if (!G->f7) {
                        if (!G->f4)
                            RfuManager_NotifyEvent(0x21, 0);
                    } else if (G->f7 != 7) {
                        G->f4 = 5;
                        G->f7 = 5;
                    }
                }
                break;
            case 0x1f:
                if (!result) {
                    G->f5 = 13;
                    G->f4 = 13;
                }
                break;
            case 0x20:
                if (!result && !rfu_getConnectParentStatus(&status, &G->f10) && !status)
                    G->f4 = 14;
                if (G->f1a && --G->f1a == 0)
                    G->f4 = 14;
                break;
            case 0x21:
                if (!result && !rfu_getConnectParentStatus(&status, &G->f10)) {
                    if (!status) {
                        G->f4 = 19;
                        G->f5 = 15;
                        G->f1e = 0x22;
                        G->f14 = G->f10;
                    } else {
                        G->f5 = 0;
                        G->f4 = 0;
                        G->f1e = 0x23;
                        G->f14 = status;
                        if (G->f7) {
                            G->f7 = 3;
                            G->f4 = 9;
                        }
                    }
                    RfuManager_NotifyEvent((u8)G->f1e, 1);
                    G->f1e = 0;
                }
                break;
            case 0x32:
                if (!result) {
                    G->f14 = gRfuLinkStatus->linkLossSlotFlag;
                    G->f5 = 17;
                    G->f4 = 17;
                    G->f10 = 0;
                    while (!((gRfuLinkStatus->linkLossSlotFlag >> G->f10) & 1)) {
                        if (++G->f10 > 3)
                            break;
                    }
                }
                break;
            case 0x33:
                if (!result && !rfu_CHILD_getConnectRecoveryStatus(&status) && status <= 1)
                    G->f4 = 18;
                if (G->a34[G->f10] && --G->a34[G->f10] == 0)
                    G->f4 = 18;
                break;
            case 0x34:
                if (!result && !rfu_CHILD_getConnectRecoveryStatus(&status)) {
                    if (!status) {
                        G->f4 = 19;
                        G->f5 = 22;
                        G->f1e = 0x32;
                    } else {
                        G->f5 = 0;
                        G->f4 = 0;
                        RfuManager_DisconnectAndWait(gRfuLinkStatus->linkLossSlotFlag);
                        G->f1e = 0x33;
                    }
                    G->a34[G->f10] = 0;
                    G->f30 = 0;
                    G->fa = 0;
                    RfuManager_NotifyEvent((u8)G->f1e, 1);
                    G->f1e = 0;
                }
                break;
            case 0x27:
                if (!result) {
                    if (G->f5 == 22) {
                        G->f4 = G->f11;
                        G->f5 = G->f12;
                        G->f2 = 1;
                        RfuManager_NotifyEvent(0x41, 0);
                    } else if (G->f5 == 15) {
                        G->f4 = G->f5;
                        G->f2 = 1;
                        RfuManager_NotifyEvent(0x41, 0);
                        G->f24 |= 1 << G->f10;
                        G->a28[G->f10] = G->f26;
                        rfu_clearSlot(4, G->f10);
                        status = rfu_NI_CHILD_setSendGameName(G->f10, 14);
                        if (status) {
                            G->f5 = 0;
                            G->f4 = 0;
                            RfuManager_CompletePendingMscCycle();
                            RfuManager_DisconnectAndWait(gRfuLinkStatus->connSlotFlag |
                                                         gRfuLinkStatus->linkLossSlotFlag);
                            G->f14 = status;
                            RfuManager_NotifyEvent(0x25, 1);
                        }
                    }
                }
                break;
            case 0x3d:
                if (!result) {
                    G->f5 = 0;
                    G->f4 = 0;
                    RfuManager_NotifyEvent(0x42, 0);
                }
                break;
        }
        G->fe = 1;
    } else if (result == 3 && G->ff && (command == 0x24 || command == 0x26 || command == 0x27)) {
        rfu_REQ_RFUStatus();
        rfu_waitREQComplete();
        rfu_getRFUStatus(&status);
        if (!status && gRfuLinkStatus->parentChild == 0) {
            u8 *buf = rfu_getSTWIRecvBuffer();
            buf[4] = gRfuLinkStatus->connSlotFlag;
            buf[5] = 1;
            RfuManager_UpdateLinkRecovery(0x29);
            result = 0;
        }
    }
    switch (command) {
        case 0x30:
            if (!result) {
                G->f14 = rfu_getSTWIRecvBuffer()[8];
                RfuManager_RemoveSendTargets();
                if (G->f30) {
                    G->f30 &= ~(u8)G->f14;
                    for (i = 0; i < 4; i++)
                        if ((G->f14 >> i) & 1)
                            G->a34[i] = 0;
                    if (G->f6 == 0) {
                        G->f5 = 0;
                        G->f4 = 0;
                    }
                }
                status = (u8)G->f14 & G->f0;
                for (i = 0; i < 4; i++)
                    if (((status >> i) & 1) && G->f1)
                        --G->f1;
                G->f0 &= ~(u8)G->f14;
                if (G->f7 && gRfuLinkStatus->parentChild == 0xff) {
                    if (G->f7 == 8) {
                        G->f1a = G->f1c;
                        G->f7 = 6;
                        G->f4 = 6;
                    } else if ((u8)(G->f4 - 6) > 1) {
                        G->f7 = 1;
                        G->f4 = 5;
                    }
                }
                if (gRfuLinkStatus->parentChild == 0xff && !G->f4)
                    G->f6 = 0xff;
                if (!G->fe)
                    RfuManager_NotifyEvent(0x40, 1);
            }
            break;
        case 0x26:
            RfuManager_FinishSlotReception();
            if (gRfuLinkStatus->parentChild != 0xff)
                RfuManager_NotifyEvent(0x50, 0);
            break;
        case 0x10:
        case 0x3d:
            if (!result) {
                G->fd = 0;
                G->f1 = 0;
                G->f0 = 0;
                G->f6 = 0xff;
                RfuManager_CompletePendingMscCycle();
                if (command == 0x3d)
                    RfuManager_ClearState();
            }
            break;
    }
    if (result) {
        s = (void *)0x03005390;
        if (command == 0x1c && result && G->f7 == 4) {
            gRfuLinkStatus->parentChild = 1;
            gRfuLinkStatus->connSlotFlag = 15;
            RfuManager_DisconnectAndWait(15);
            rfu_waitREQComplete();
            return;
        }
        G->f14 = command;
        G->f16 = result;
        if (G->fe) {
            G->f5 = 0;
            G->f4 = 0;
        }
        RfuManager_NotifyEvent(0xf0, 2);
        RfuManager_CompletePendingMscCycle();
    }
    if (command == 0xff) {
        RfuManager_NotifyEvent(0xf2, 0);
        RfuManager_CompletePendingMscCycle();
    }
}

#else
INCLUDE_ASM("asm/nonmatching", RfuManager_ApplyRequestResult);
#endif
