#include "global.h"
#include "rfu_manager.h"
struct L {
    u8 pad[2];
    u8 f2;
    u8 f3;
};
extern u16 *gUnk_03006A70[];
extern struct L *gUnk_03006A80;
void rfu_clearSlot(u32, u32);
void RfuManager_NotifyEvent(u32, u32);
void RfuManager_RequestMscCycleCompletion(void);
void RfuManager_DisconnectAndWait(u32);
// confidence=low: counts down the slot timeout and tears the link down when it expires.
void sub_082247CC(void)
{
    u16 ime = *(vu16 *)0x04000208;
    *(vu16 *)0x04000208 = 0;
    if (gRfuLinkManager.state == 0xf) {
        if (--gRfuLinkManager.nameAcceptTimer.count[gRfuLinkManager.child_slot] == 0 ||
            *gUnk_03006A70[gRfuLinkManager.child_slot] == 0x27) {
            RfuManager_RequestMscCycleCompletion();
            gRfuLinkManager.state = 0x18;
            rfu_clearSlot(4, gRfuLinkManager.child_slot);
            gRfuLinkManager.nameAcceptTimer.active &= ~(1 << gRfuLinkManager.child_slot);
            gRfuLinkManager.nameAcceptTimer.count[gRfuLinkManager.child_slot] = 0;
        }
    }
    *(vu16 *)0x04000208 = ime;
    if (gRfuLinkManager.state == 0x18) {
        if (gRfuLinkManager.childClockSlave_flag == 1)
            RfuManager_RequestMscCycleCompletion();
        if (gRfuLinkManager.childClockSlave_flag == 0) {
            gRfuLinkManager.next_state = 0;
            gRfuLinkManager.state = 0;
            RfuManager_DisconnectAndWait(gUnk_03006A80->f2 | gUnk_03006A80->f3);
            gRfuLinkManager.event.words.code = 0;
            RfuManager_NotifyEvent(0x25, 1);
        }
    }
}
