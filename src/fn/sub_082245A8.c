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
// fuzzy: 82.1%
void RfuManager_UpdateChildAdmissions(void)
{
    struct Manager *state = G;
    struct Manager *s;
    if ((u8)(state->f4 - 5) <= 3) {
        u8 connected = gRfuLinkStatus->connSlotFlag;
        u8 added = state->fc ^ connected;
        u32 accepted;
        u8 i;
        u8 *pending;
        added &= connected;
        added &= ~gRfuLinkStatus->getNameFlag;
        state->fc = connected;
        if (added) {
            state->f14 = added;
            RfuManager_NotifyEvent(0x10, 1);
        }
        accepted = 0;
        i = 0;
        s = state;
        pending = &s->f24;
        for (; i < 4; i++) {
            u8 bit = 1 << i;
            u8 action = 0;
            if (added & bit) {
                s->a28[i] = s->f26;
                *pending |= bit;
            } else if (*pending & bit) {
                if (gRfuSlotStatusNI[i]->recv.state == 0x46) {
                    if (gRfuSlotStatusNI[i]->recv.dataType == 1) {
                        const u16 *ids = state->allowed;
                        action = 2;
                        while (*ids != 0xffff) {
                            if (gRfuLinkStatus->partner[i].serialNo == *ids) {
                                s->f0 |= bit;
                                s->f1++;
                                accepted |= bit;
                                action |= 1;
                                break;
                            }
                            ids++;
                        }
                        if (!(action & 1))
                            action |= 4;
                    }
                } else if (--state->a28[i] == 0)
                    action = 6;
                if (action & 2) {
                    *pending &= ~bit;
                    s->a28[i] = 0;
                    rfu_clearSlot(8, i);
                }
                state = G;
                if (action & 4)
                    s->fd |= bit;
            }
        }
        if (accepted) {
            G->f14 = accepted;
            RfuManager_NotifyEvent(0x11, 1);
        }
        if (G->fd) {
            u8 ready = 1;
            if (gRfuLinkStatus->sendSlotUNIFlag && ((G->f3 & G->f0) != G->f0))
                ready = 0;
            if (ready) {
                RfuManager_DisconnectAndWait(G->fd);
                G->f14 = G->fd;
                G->fd = 0;
                RfuManager_NotifyEvent(0x12, 1);
            }
        }
        state = G;
        if (!state->f24 && state->f4 == 8) {
            if (!G->f7) {
                G->f5 = 0;
                G->f4 = 0;
                RfuManager_NotifyEvent(0x14, 0);
            } else {
                if (G->f7 == 2) {
                    G->f7 = 3;
                    G->f4 = 9;
                } else {
                    G->f7 = 1;
                    G->f4 = 5;
                }
                if (G->f0) {
                    G->f1a = 0;
                    G->f7 = 8;
                    G->f4 = 5;
                }
            }
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", RfuManager_UpdateChildAdmissions);
#endif
