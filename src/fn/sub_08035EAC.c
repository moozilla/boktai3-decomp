#include "global.h"

void rfu_REQ_recvData(void);

void Wireless_OnParentNiSendComplete(void)
{
    rfu_REQ_recvData();
}
