#include "global.h"
#include "system/game_state/game_state.h"
#include "link/wireless/wireless_trade_menu.h"

void WirelessGameMenu_ConsumeSelectedEquipment(struct WirelessGameExchange *p)
{
    struct GameState *save = gGameState;
    int item = *(save->slots88 + save->selected);

    if (item >= 0) {
        int flags = p->equipmentMask & ~(1 << item);
        p->equipmentMask = flags;
        if (item == 1 || item == 2)
            p->equipmentMask = flags & ~6;
    }
}

void WirelessGameMenu_UpdateIdle(struct WirelessGameExchange *p)
{
    if (p->entered)
        p->entered = 0;
}
