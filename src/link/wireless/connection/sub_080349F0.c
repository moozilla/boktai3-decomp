#include "global.h"

void rfu_REQ_recvData(void);
void WirelessController_OnReceiveRequest(void)
{
    rfu_REQ_recvData();
}
