#include "wireless_trade_menu.h"

void WirelessGameMenu_OnResultPanelClosed(void)
{
    struct WirelessGameExchange *menu = WirelessExchange_Get();
    if (menu)
        menu->resultPanelClosed = 1;
}
