#include "engine/shared/global.h"

struct S {
    u8 f[0x21];
    u8 a;
};
u32 Wireless_ReceiveChildUniPacket(struct S *);
void rfu_UNI_readySendData(u32);
void RfuManager_SendData(u32);
void Wireless_ProcessChildUniExchange(struct S *p)
{
    if (Wireless_ReceiveChildUniPacket(p))
        rfu_UNI_readySendData(p->a);
    RfuManager_SendData(1);
}
