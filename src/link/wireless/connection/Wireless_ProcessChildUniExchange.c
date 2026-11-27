#include "global.h"
#include "link/wireless/wireless_controller.h"

u32 Wireless_ReceiveChildUniPacket(struct Engine *);
void rfu_UNI_readySendData(u32);
void RfuManager_SendData(u32);
void Wireless_ProcessChildUniExchange(struct Engine *p)
{
    if (Wireless_ReceiveChildUniPacket(p))
        rfu_UNI_readySendData(p->parentSlot);
    RfuManager_SendData(1);
}
