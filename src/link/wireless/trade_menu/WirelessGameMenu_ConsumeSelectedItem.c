#include "global.h"
#include "system/game_state/game_state.h"
#include "link/wireless/wireless_trade_menu.h"

void WirelessGameMenu_ConsumeSelectedItem(struct WirelessGameExchange *p)
{
    struct GameState *save = gGameState;
    s16 *selected = (s16 *)&save->unk50;
    int item;

    if (*selected > 7) {
        item = *(save->inventory + *selected);
        if (item >= 0) {
            p->extraItemMask &= ~(1 << item);
        }
    } else {
        item = *(save->inventory + *selected);
        p->itemMask &= ~(1 << item);
    }
}
