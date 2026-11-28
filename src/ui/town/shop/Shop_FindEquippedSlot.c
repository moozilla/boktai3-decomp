#include "ui/town/shop_actions.h"
#include "system/game_state/game_state.h"
s32 Shop_FindEquippedSlot(s32 selected)
{
    s32 i;
    for (i = 0; i < 8; i++) {
        if (gGameState->inventory[i] == selected)
            return i;
    }
    return -1;
}
