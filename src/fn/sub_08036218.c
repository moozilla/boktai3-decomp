#include "global.h"

extern u32 gUnk_02000484;
void rfu_REQ_recvData(void);
void Wireless_OnChildTransferReceiveRequest(void)
{
    if (gUnk_02000484)
        rfu_REQ_recvData();
}
