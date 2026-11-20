#include "wireless_trade_menu.h"

void WirelessGameDisplay_UpdateStatusIcons(struct WirelessGameDisplay *);

void WirelessGameMenu_UpdateHealthIndicators(struct WirelessGameExchange *menu)
{
    if (menu->display)
        WirelessGameDisplay_UpdateStatusIcons(menu->display);
}
