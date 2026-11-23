#include "wireless_trade_menu.h"
#include "global.h"
#include "render.h"
#include "game_state.h"
#include "engine_pause_flags.h"
void WirelessGameMenu_SetStatusPanelState(struct WirelessGameExchange *, int);
void WirelessGameMenu_SignalStatusPanel(struct WirelessGameExchange *);
void Player_EnterStateTen(void);
void Player_EnterStateEleven(void);
void WirelessGame_GetSelectedEquipmentEffect(int);
void Player_SetTransitionMode(void);
void WirelessGameMenu_UpdateRoundPreparation(struct WirelessGameExchange *p)
{
    if (p->entered) {
        p->entered = 0;
        p->roundCount++;
        if (p->roundCount > 98)
            p->roundCount = 99;
        WirelessGameMenu_SetStatusPanelState(p, 3);
    }
    if (p->timer > 89) {
        WirelessGameMenu_EnterState(p, WIRELESS_MENU_ACTIVE_ROUND);
        Engine_ClearPauseFlags(4);
        WirelessGameMenu_SignalStatusPanel(p);
        switch (p->equipmentMode) {
            case 0:
                if (*(gGameState->slots88 + gGameState->selected) == 1) {
                    p->equipmentMode = 2;
                    Player_EnterStateEleven();
                } else if (*(gGameState->slots88 + gGameState->selected) == 2) {
                    p->equipmentMode = 1;
                    Player_EnterStateTen();
                }
                break;
            case 2:
                if (*(gGameState->slots88 + gGameState->selected) == 2) {
                    p->equipmentMode = 1;
                    Player_EnterStateTen();
                } else {
                    p->equipmentMode = 0;
                    Player_EnterStateEleven();
                }
                break;
            case 1:
                if (*(gGameState->slots88 + gGameState->selected) == 1) {
                    p->equipmentMode = 2;
                    Player_EnterStateEleven();
                } else {
                    p->equipmentMode = 0;
                    Player_EnterStateTen();
                }
                break;
        }
        WirelessGame_GetSelectedEquipmentEffect(*(gGameState->slots88 + gGameState->selected));
        Player_SetTransitionMode();
    } else
        p->timer++;
}
