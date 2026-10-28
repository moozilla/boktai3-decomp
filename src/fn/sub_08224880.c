#include "global.h"
#include "rfu_manager.h"
extern u16 *gRfuSlotStatusNI[];
void rfu_clearSlot(u32, u32);
void RfuManager_NotifyEvent(u32, u32);
void RfuManager_FinishSlotReception(void)
{
    struct RfuLinkManager *s = &gRfuLinkManager;
    if (s->state == 0xf) {
        u16 **t = gRfuSlotStatusNI;
        u32 i = s->child_slot;
        if (*t[i] == 0x26) {
            u32 z = 0;
            s->next_state = z;
            s->state = z;
            rfu_clearSlot(4, i);
            s->nameAcceptTimer.active &= ~(1 << s->child_slot);
            s->nameAcceptTimer.count[s->child_slot] = z;
            RfuManager_NotifyEvent(0x24, 0);
        }
    }
}
