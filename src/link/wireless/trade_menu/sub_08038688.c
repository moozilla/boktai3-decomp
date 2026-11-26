#include "link/wireless/wireless_trade_menu.h"
#include "player/player.h"

void PlayerCue_ReleaseControl(struct Player *);

void WirelessGameMenu_UpdateStartupDelay(struct WirelessGameExchange *p)
{
    if (p->entered)
        p->entered = 0;
    if (p->timer > 1) {
        WirelessGameMenu_EnterState(p, WIRELESS_MENU_ROUND_HANDSHAKE);
        PlayerCue_ReleaseControl(gPlayers[0]);
    }
    p->timer++;
}
