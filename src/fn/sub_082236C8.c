#include "global.h"
#include "rfu_manager.h"

u32 rfu_getMasterSlave(void);
void RfuManager_NotifyEvent(u32, u32);

u32 RfuManager_StartConnect(u8 a, u16 b, u16 c, u16 *list)
{
    u8 i;
    u16 *p;
    if (gRfuLinkManager.state != 0 && (gRfuLinkManager.state != 8 || a != 1)) {
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
    p = list;
    if (*p++ != 0xFFFF) {
        do {
            i++;
            if (i > 0xf)
                break;
        } while (*p++ != 0xFFFF);
    }
    if (i == 0x10) {
        gRfuLinkManager.event.words.code = 4;
        RfuManager_NotifyEvent(0xf3, 1);
        return 4;
    }
    if (a > 1) {
        gRfuLinkManager.pcswitch_flag = 1;
        a = 1;
        b = 0;
    } else {
        gRfuLinkManager.pcswitch_flag = 0;
    }
    if (a != 0) {
        gRfuLinkManager.state = 5;
    } else {
        gRfuLinkManager.state = 9;
        if (gRfuLinkManager.fastSearchParent_flag != 0)
            gRfuLinkManager.fastSearchParent_flag = 2;
    }
    gRfuLinkManager.parent_child = a;
    gRfuLinkManager.connect_period = b;
    gRfuLinkManager.nameAcceptTimer.count_max = c;
    gRfuLinkManager.acceptable_serialNo_list = list;
    return 0;
}
