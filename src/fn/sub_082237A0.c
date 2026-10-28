#include "global.h"
#include "rfu_manager.h"

struct RfuLink {
    u8 pad0[8];
    u8 count;
    u8 pad9[11];
    u16 id;
    u8 pad16[10];
};
extern struct RfuLink *gUnk_03006A80;
u32 rfu_getMasterSlave(void);
void RfuManager_NotifyEvent(u32, u32);

u32 RfuManager_StartChildConnect(u16 id, u16 b)
{
    u8 i;
    u32 count;
    if (gRfuLinkManager.state != 0 && (u8)(gRfuLinkManager.state - 9) > 2) {
        gRfuLinkManager.event.words.code = 1;
        RfuManager_NotifyEvent(0xf3, 1);
        return 1;
    }
    if ((u8)rfu_getMasterSlave() == 0) {
        gRfuLinkManager.event.words.code = 2;
        RfuManager_NotifyEvent(0xf3, 1);
        return 2;
    }
    i = 0;
    *(&count) = gUnk_03006A80->count;
    if (i < count && gUnk_03006A80[0].id != id) {
        struct RfuLink *l = gUnk_03006A80;
        do {
            i++;
            if (i >= count)
                break;
        } while (((struct RfuLink *)((u8 *)l + (i << 5)))->id != id);
    }
    count = gUnk_03006A80->count;
    if (count == 0 || i == count) {
        gRfuLinkManager.event.words.code = 3;
        RfuManager_NotifyEvent(0xf3, 1);
        return 3;
    }
    if (gRfuLinkManager.state == 0 || gRfuLinkManager.state == 9) {
        do {
            gRfuLinkManager.state = 0xc;
            gRfuLinkManager.next_state = 0xd;
        } while (0);
    } else {
        gRfuLinkManager.state = 0xb;
        gRfuLinkManager.next_state = 0xc;
    }
    gRfuLinkManager.work = id;
    gRfuLinkManager.connect_period = b;
    if (gRfuLinkManager.pcswitch_flag != 0)
        gRfuLinkManager.pcswitch_flag = 7;
    return 0;
}
