#include "wireless_trade_menu.h"
#include "global.h"
#include "render.h"
#include "game_state.h"
void WirelessGameMenu_SetStatusPanelState(struct WirelessGameExchange *, int);
void WirelessGameMenu_SignalStatusPanel(struct WirelessGameExchange *);
void Player_EnterStateTen(void);
void Player_EnterStateEleven(void);
s32 WirelessGame_GetSelectedEquipmentEffect(int);
void Player_SetTransitionMode(s32);
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
        u32 seven = 7;
        u8 *state = &p->state;
        u32 zero = 0;
        u32 one;
        u8 *entered;
        u8 *mode;
        *state = seven;
        entered = &p->entered;
        one = 1;
        *entered = one;
        p->timer = zero;
        {
            u32 *global = &gEnginePauseFlags;
            u32 mask = ~4;
            *global &= mask;
        }
        WirelessGameMenu_SignalStatusPanel(p);
        mode = &p->equipmentMode;
        switch (*mode) {
            case 0: {
                int n = *(gGameState->slots88 + gGameState->selected);
                if (n == 1)
                    goto mode2;
                if (n != 2)
                    goto done;
                goto mode1;
            }
            case 2:
                if (*(gGameState->slots88 + gGameState->selected) == 2) {
                mode1:
                    *mode = one;
                    Player_EnterStateTen();
                } else {
                    *mode = 0;
                    Player_EnterStateEleven();
                }
                break;
            case 1:
                if (*(gGameState->slots88 + gGameState->selected) == 1) {
                mode2:
                    *mode = 2;
                    Player_EnterStateEleven();
                } else {
                    *mode = 0;
                    Player_EnterStateTen();
                }
                break;
        }
    done:
        Player_SetTransitionMode(
            WirelessGame_GetSelectedEquipmentEffect(*(gGameState->slots88 + gGameState->selected)));
    } else
        p->timer++;
}
