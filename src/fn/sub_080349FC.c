#include "global.h"
/* WirelessController_RequestReceiveIfCreated: Issues an RFU receive request when the controller exists */
#define WirelessController_RequestReceiveIfCreated sub_080349FC

extern void *gUnk_02000484;
void rfu_REQ_recvData(void);

void sub_080349FC(void)
{
    if (gUnk_02000484)
        rfu_REQ_recvData();
}
