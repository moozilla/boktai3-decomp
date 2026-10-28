#include "global.h"
#include "ram.h"
/* WirelessController_RequestReceiveIfCreated: Issues an RFU receive request when the controller exists */
#define WirelessController_RequestReceiveIfCreated sub_080349FC

void rfu_REQ_recvData(void);

void sub_080349FC(void)
{
    if (gWirelessController)
        rfu_REQ_recvData();
}
