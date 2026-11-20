#include "wireless_trade_menu.h"
#include "global.h"
#include "input.h"
void WirelessGameMenu_InitItemChoices(struct WirelessGameExchange *);
void WirelessGameMenu_InitEquipmentChoices(struct WirelessGameExchange *);
void SceneHud_EnterOptionControls(void);
void StatusPanel_BeginRefresh(void);
void WirelessGameMenu_SetStatusPanelState(struct WirelessGameExchange *, int);
void WirelessGameMenu_SignalStatusPanel(struct WirelessGameExchange *);
void Sound_PlaySong(int);
void WirelessGameMenu_UpdateSelection(struct WirelessGameExchange *p)
{
    if (p->entered) {
        p->entered = 0;
        WirelessGameMenu_InitItemChoices(p);
        WirelessGameMenu_InitEquipmentChoices(p);
        p->selectionRequested = 0;
        SceneHud_EnterOptionControls();
        p->selectionReady = 1;
        p->selectionCountdown = 8;
        p->choice = 6;
        WirelessGameMenu_SetStatusPanelState(p, 2);
    }
    if (gPadState[1] & 8) {
        p->selectionRequested = 1;
        p->selectionCountdown = 8;
    }
    if (p->selectionReady && p->timer > 16 && p->selectionRequested) {
        if (!p->selectionCountdown) {
            Sound_PlaySong(0xdd);
            WirelessGameMenu_EnterState(p, WIRELESS_MENU_SELECTION_EXCHANGE);
            StatusPanel_BeginRefresh();
            WirelessGameMenu_SignalStatusPanel(p);
            if (p->itemMode && p->enabled)
                p->enabled--;
            return;
        }
        p->selectionCountdown--;
    }
    p->timer++;
}
