#include "global.h"
#include "engine/ram.h"
/* WirelessController_RequestReceiveIfCreated: Issues an RFU receive request when the controller exists */

void rfu_REQ_recvData(void);

void WirelessController_RequestReceiveIfCreated(void)
{
    if (gWirelessController)
        rfu_REQ_recvData();
}
