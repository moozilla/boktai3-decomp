#include "system/game_state/game_state.h"

void Inventory_SetKeyItemId(s32 i, u16 id)
{
    *(i + gGameState->unk838) = id;
}
