#include "global.h"
struct Panel {
    u8 pad0[0x60];
    void *task;
};
void WirelessGameItems_Create(int, int);
void *WirelessGameDisplay_Create(void);
void WirelessExchange_OnPacketSent(void);
void WirelessExchange_OnPacketReceived(void);
void WirelessExchange_OnSendFailure(void);
void WirelessExchange_OnReceiveFailure(void);
void Wireless_SetTransferCallbacks(void *, void *, void *, void *);
void WirelessLinkIndicator_SetPosition(int, int);
void WirelessGame_InitDisplayAndLinkCallbacks(struct Panel *p)
{
    WirelessGameItems_Create(0, 0);
    p->task = WirelessGameDisplay_Create();
    Wireless_SetTransferCallbacks(WirelessExchange_OnPacketSent, WirelessExchange_OnPacketReceived,
                                  WirelessExchange_OnSendFailure,
                                  WirelessExchange_OnReceiveFailure);
    WirelessLinkIndicator_SetPosition(4, 24);
}
