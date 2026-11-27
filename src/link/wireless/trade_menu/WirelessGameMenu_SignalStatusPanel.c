#include "link/wireless/wireless_trade_menu.h"

u32 WirelessGameDisplay_RequestCompletion(struct WirelessGameDisplay *);

void WirelessGameMenu_SignalStatusPanel(struct WirelessGameExchange *menu)
{
    if (menu->display)
        WirelessGameDisplay_RequestCompletion(menu->display);
}
