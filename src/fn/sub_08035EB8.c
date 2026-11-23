#include "global.h"
#include "wireless_controller.h"
#include "rfu_compat.h"

void Memory_Clear(void *, int);
void Bg_ClearTilemap(int);
void Wireless_InitPacketQueue(void *);
void Wireless_OnParentNiSessionEvent(u8);
void Wireless_OnParentNiSendComplete(void);
void RfuManager_SetEventCallback(void (*)(u8));
void RfuManager_SetMscCallback(void (*)(void));
void Script_Call(void *, int);
void Sound_FadeOutChannel10(int);

void Wireless_BeginParentNiSession(WirelessController *p)
{
    struct WirelessPeerRoster *roster = &p->roster;
    int i;
    Bg_ClearTilemap(0);
    Wireless_InitPacketQueue(p->packetQueue);
    Memory_Clear(p->niSend, sizeof(p->niSend));
    Memory_Clear(p->niReceive, sizeof(p->niReceive));
    rfu_clearSlot(8, p->sendSlot);
    RfuManager_SetEventCallback(Wireless_OnParentNiSessionEvent);
    RfuManager_SetMscCallback(Wireless_OnParentNiSendComplete);
    for (i = 0; i <= 3; i++) {
        if (roster->slots & (2 << i))
            rfu_setRecvBuffer(32, i, p->niReceive, sizeof(p->niReceive));
    }
    p->sessionMode = 2;
    WirelessController_SetOperation(p, 23);
    if (p->connectionScript)
        Script_Call(p->connectionScript, 0);
    Sound_FadeOutChannel10(4);
}
