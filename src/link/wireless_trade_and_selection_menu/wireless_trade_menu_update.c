#include "link/wireless_trade_and_selection_menu/wireless_trade_menu.h"

int Wireless_GetStatus(void);
int Wireless_QueueNiPacket(u8 *);

extern void (*const gUnk_086057A0[])(struct WirelessGameExchange *);

int WirelessTradeMenu_Update(struct WirelessGameExchange *menu)
{
    int busy = Wireless_GetStatus();
    if (!busy) {
        gUnk_086057A0[menu->state](menu);
        if (menu->pendingPacket) {
            if (Wireless_QueueNiPacket(menu->sendPacket) >= 0)
                menu->pendingPacket = 0;
        }
    }
    return 0;
}
