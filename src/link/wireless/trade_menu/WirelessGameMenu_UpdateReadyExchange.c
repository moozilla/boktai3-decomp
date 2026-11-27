#include "link/wireless/wireless_trade_menu.h"
#include "global.h"
#include "engine/word_flags.h"
int WirelessExchange_SendPacket(struct WirelessGameExchange *, int, const u32 *, int);
void WirelessGameMenu_UpdateReadyExchange(struct WirelessGameExchange *p)
{
    u32 data;
    u32 retry;
    if (p->entered) {
        p->entered = 0;
        p->readyToSend = 0;
        data = 0;
        WirelessExchange_SendPacket(p, 3, &data, 1);
    }
    if (p->readyToSend && p->selectionReady) {
        retry = 0;
        WirelessExchange_SendPacket(p, 3, &retry, 1);
        p->selectionReady = 0;
    }
    if (WordFlags_Test(&p->exchangeFlags, WIRELESS_EXCHANGE_READY) == WIRELESS_EXCHANGE_READY) {
        p->exchangeFlags &= ~WIRELESS_EXCHANGE_READY;
        WirelessGameMenu_EnterState(p, WIRELESS_MENU_ROUND_PREPARATION);
    } else
        p->timer++;
}
