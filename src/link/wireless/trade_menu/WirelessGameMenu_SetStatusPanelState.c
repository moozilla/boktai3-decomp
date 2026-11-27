#include "link/wireless/wireless_trade_menu.h"

int WirelessGameDisplay_SetState(struct WirelessGameDisplay *, int);

void WirelessGameMenu_SetStatusPanelState(struct WirelessGameExchange *menu, int state)
{
    if (menu->display)
        WirelessGameDisplay_SetState(menu->display, state);
}
