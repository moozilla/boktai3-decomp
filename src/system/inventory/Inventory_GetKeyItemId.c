#include "system/game_state/game_state.h"

s32 Inventory_GetKeyItemId(s32 i)
{
    return *(s16 *)&gGameState->unk838[i];
}
