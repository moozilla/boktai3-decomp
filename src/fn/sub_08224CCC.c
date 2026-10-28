#include "global.h"
#define gRfuLinkStatus gUnk_03006A80
#include "rfu_compat.h"
#include "rfu_manager.h"
#define G (&gRfuLinkManager)
struct RfuLinkManager;
void RfuManager_NotifyEvent(u8, u8);
void RfuManager_DisconnectAndWait(u8);

void sub_08224CCC(void)
{
    if (G->pcswitch_flag) {
        switch (G->state) {
            case 5:
                G->pcswitch_flag = 3;
                G->state = 9;
                break;
            case 6:
                G->pcswitch_flag = 2;
                G->connect_period = 1;
                break;
            case 7:
            case 8:
                G->pcswitch_flag = 2;
                break;
            case 9:
            case 10:
                G->connect_period = 40;
                break;
            case 11:
                G->connect_period = 40;
                G->state = 10;
                break;
        }
    }
}
