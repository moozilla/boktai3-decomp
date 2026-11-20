#include "wireless_controller.h"
void Memory_Clear(void *, int);
#include "rfu_compat.h"
#include "ram.h"
void Bg_ClearTilemap(int);
void Wireless_InitPacketQueue(void *);
void Wireless_OnChildTransferEvent(u8);
void Wireless_OnChildTransferReceiveRequest(void);
void RfuManager_SetEventCallback(void (*)(u8));
void RfuManager_SetMscCallback(void (*)(void));
void Script_Call(void *, int);
void Wireless_BeginChildTransfer(WirelessController *p)
{
    int index;
    u8 *buffer;
    void *listener;
    Bg_ClearTilemap(0);
    for (index = 0; index < RFU_CHILD_MAX; index++) {
        if (p->roster.ids[index] == gRfuLinkStatus->my.id)
            break;
    }
    p->localPeerIndex = index;
    Wireless_InitPacketQueue(p->packetQueue);
    Memory_Clear(p->niSend, sizeof(p->niSend));
    buffer = p->niReceive;
    Memory_Clear(buffer, sizeof(p->niReceive));
    RfuManager_SetEventCallback(Wireless_OnChildTransferEvent);
    RfuManager_SetMscCallback(Wireless_OnChildTransferReceiveRequest);
    rfu_setRecvBuffer(TYPE_NI, p->parentSlot, buffer, sizeof(p->niReceive));
    p->sessionMode = 2;
    WirelessController_SetOperation(p, 24);
    listener = p->connectionScript;
    if (listener) {
        Script_Call(listener, 0);
    }
}
