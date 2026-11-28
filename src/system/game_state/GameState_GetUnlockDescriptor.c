#include "global.h"
#include "system/game_state/game_state.h"
extern u32 gUnk_08E60F48[];
u32 *GameState_GetUnlockDescriptor(u32 i)
{
    return &gUnk_08E60F48[*(i + gGameState->table4C0)];
}
