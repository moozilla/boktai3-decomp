#include "wireless_trade_menu.h"
#include "global.h"
void Wireless_RequestResetConnection(void);
void WirelessGameMenu_SignalStatusPanel(struct WirelessGameExchange *);
void Script_Call(u32, int);
void WirelessGameMenu_UpdateResultsDelay(struct WirelessGameExchange *p)
{
    if (p->entered)
        p->entered = 0;
    if (p->timer > 119) {
        Wireless_RequestResetConnection();
        WirelessGameMenu_EnterState(p, WIRELESS_MENU_RESULTS_IDLE);
        WirelessGameMenu_SignalStatusPanel(p);
        if (p->resultsScript)
            Script_Call(p->resultsScript, 0);
    } else
        p->timer++;
}
