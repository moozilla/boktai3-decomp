#include "global.h"
#define gRfuLinkStatus gUnk_03006A80
#include "rfu_compat.h"
struct Manager {
    u8 f0, f1;
    volatile u8 f2;
    u8 f3, f4, f5, f6, f7, f8, f9, fa, fb, fc, fd, fe, ff;
    u8 f10, f11, f12, p13;
    u16 f14, f16, f18, f1a, f1c, f1e;
};
extern struct Manager gUnk_03005390;
void RfuManager_ResetConnectionState(void);
void RfuManager_NotifyEvent(u8, u8);
void RfuManager_DisconnectAndWait(u8);

/* Cancels the current RFU operation: with a nonzero argument resets the
   connection outright, otherwise steps the state back and reports it. */
void sub_082238F0(u8 reset)
{
    struct Manager *m;
    u8 event = 0;
    gUnk_03005390.f7 = event;
    m = &gUnk_03005390;
    if (reset) {
        RfuManager_ResetConnectionState();
        m->f4 = 0x17;
        return;
    }
    switch (m->f4) {
        case 5:
            m->f4 = 8;
            m->f5 = 0;
            event = 0x13;
            break;
        case 6:
        case 7:
            m->f4 = 7;
            m->f5 = 8;
            break;
        case 8:
        case 15:
            break;
        case 9:
            m->f5 = 0;
            m->f4 = 0;
            event = 0x21;
            break;
        case 10:
        case 11:
            m->f4 = 0xB;
            m->f5 = 0;
            break;
        case 12:
            m->f5 = 0;
            m->f4 = 0;
            event = 0x23;
            break;
        case 13:
        case 14:
            m->f4 = 0xE;
            break;
        case 16:
            m->f4 = m->f11;
            m->f5 = m->f12;
            RfuManager_DisconnectAndWait(gRfuLinkStatus->linkLossSlotFlag);
            m->f14 = gRfuLinkStatus->linkLossSlotFlag;
            RfuManager_NotifyEvent(0x33, 1);
            return;
        case 17:
        case 18:
            m->f4 = 0x12;
            break;
        default:
            m->f5 = 0;
            m->f4 = 0;
            event = 0x43;
            break;
    }
    if (m->f4 == 0)
        RfuManager_NotifyEvent(event, 0);
}
