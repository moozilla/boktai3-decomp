#include "global.h"
#include "rfu_manager.h"

u32 sub_08249498(u32, u32);

void RfuManager_StepSlotTimer(u32 a)
{
    if (gRfuLinkManager.pcswitch_flag == 5) {
        gRfuLinkManager.parent_child = 1;
        gRfuLinkManager.state = 5;
        gRfuLinkManager.connect_period = gRfuLinkManager.pcswitch_period_bak;
        if (gRfuLinkManager.connect_period != 0) {
            gRfuLinkManager.pcswitch_flag = 6;
        } else {
            gRfuLinkManager.pcswitch_flag = 1;
        }
    }
    if (gRfuLinkManager.pcswitch_flag == 1) {
        gRfuLinkManager.parent_child = 1;
        gRfuLinkManager.state = 5;
        gRfuLinkManager.connect_period = sub_08249498(a, 0x8c);
        gRfuLinkManager.pcswitch_period_bak = 0x8c - gRfuLinkManager.connect_period;
        if (gRfuLinkManager.connect_period != 0) {
            gRfuLinkManager.pcswitch_flag = 2;
        } else {
            gRfuLinkManager.pcswitch_flag = 3;
        }
    }
    if (gRfuLinkManager.pcswitch_flag == 3) {
        gRfuLinkManager.parent_child = 0;
        gRfuLinkManager.connect_period = 0x28;
        gRfuLinkManager.pcswitch_flag = 4;
        gRfuLinkManager.state = 9;
    }
}
