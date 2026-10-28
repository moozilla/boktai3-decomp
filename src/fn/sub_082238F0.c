#include "global.h"
#include "rfu_compat.h"
#include "rfu_manager.h"
void RfuManager_ResetConnectionState(void);
void RfuManager_NotifyEvent(u8, u8);
void RfuManager_DisconnectAndWait(u8);

/* Cancels the current RFU operation: with a nonzero argument resets the
   connection outright, otherwise steps the state back and reports it. */
void sub_082238F0(u8 reset)
{
    struct RfuLinkManager *m;
    u8 event = 0;
    gRfuLinkManager.pcswitch_flag = event;
    m = &gRfuLinkManager;
    if (reset) {
        RfuManager_ResetConnectionState();
        m->state = 0x17;
        return;
    }
    switch (m->state) {
        case 5:
            m->state = 8;
            m->next_state = 0;
            event = 0x13;
            break;
        case 6:
        case 7:
            m->state = 7;
            m->next_state = 8;
            break;
        case 8:
        case 15:
            break;
        case 9:
            m->next_state = 0;
            m->state = 0;
            event = 0x21;
            break;
        case 10:
        case 11:
            m->state = 0xB;
            m->next_state = 0;
            break;
        case 12:
            m->next_state = 0;
            m->state = 0;
            event = 0x23;
            break;
        case 13:
        case 14:
            m->state = 0xE;
            break;
        case 16:
            m->state = m->state_bak[0];
            m->next_state = m->state_bak[1];
            RfuManager_DisconnectAndWait(gRfuLinkStatus->linkLossSlotFlag);
            m->event.words.code = gRfuLinkStatus->linkLossSlotFlag;
            RfuManager_NotifyEvent(0x33, 1);
            return;
        case 17:
        case 18:
            m->state = 0x12;
            break;
        default:
            m->next_state = 0;
            m->state = 0;
            event = 0x43;
            break;
    }
    if (m->state == 0)
        RfuManager_NotifyEvent(event, 0);
}
