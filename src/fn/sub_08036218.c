#include "global.h"
#include "ram.h"

void rfu_REQ_recvData(void);
void Wireless_OnChildTransferReceiveRequest(void)
{
    if (gWirelessController)
        rfu_REQ_recvData();
}
