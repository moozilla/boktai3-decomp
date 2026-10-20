#include "global.h"
#define gRfuSlotStatusNI gUnk_03006A70
#define gRfuLinkStatus   gUnk_03006A80
#include "rfu_compat.h"
#define G ((struct Manager *)0x03005390)
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
#ifdef NONMATCHING
// fuzzy: 80.7%
u32 RfuManager_UpdateLinkRecovery(u16 command)
{
    u8 loss, reason, recovered;
    u32 disconnected = 0;
    u8 i;
    rfu_REQBN_watchLink(command, &loss, &reason, &recovered);
    if (loss) {
        struct Manager *s = G;
        s->f14 = loss;
        s->f16 = reason;
        if (s->f9) {
            s->fa = 1;
            if (!s->f6 && !reason)
                s->fa = 4;
            if (s->fa == 1) {
                for (i = 0; i < 4; i++) {
                    if ((loss >> i) & 1) {
                        s->f30 |= 1 << i;
                        s->a34[i] = s->f32;
                    }
                }
                RfuManager_NotifyEvent(0x31, 1);
            } else {
                s->fa = 0;
                RfuManager_DisconnectAndWait(loss);
                disconnected = 1;
                RfuManager_NotifyEvent(0x33, 1);
            }
        } else {
            RfuManager_DisconnectAndWait(loss);
            disconnected = 1;
            RfuManager_NotifyEvent(0x30, 2);
        }
        RfuManager_CompletePendingMscCycle();
    }
    if (gRfuLinkStatus->parentChild == 1) {
        struct Manager *s;
        if (recovered) {
            for (i = 0; i < 4; i++) {
                if (((G->f30 >> i) & 1) && ((recovered >> i) & 1))
                    G->a34[i] = 0;
            }
            G->f30 &= ~recovered;
            G->f14 = recovered;
            RfuManager_NotifyEvent(0x32, 1);
        }
        s = G;
        if (s->f30) {
            u8 expired = 0;
            for (i = 0; i < 4; i++) {
                if (((s->f30 >> i) & 1) && s->a34[i] && --s->a34[i] == 0) {
                    u32 bit = 1 << i;
                    s->f30 &= ~bit;
                    expired |= bit;
                }
            }
            if (expired) {
                RfuManager_DisconnectAndWait(expired);
                disconnected = 1;
                G->f14 = expired;
                RfuManager_NotifyEvent(0x33, 1);
            }
            s = G;
            if (s->f30)
                return disconnected;
        }
        s->fa = 0;
    }
    return disconnected;
}

#else
INCLUDE_ASM("asm/nonmatching", RfuManager_UpdateLinkRecovery);
#endif
