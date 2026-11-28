#include "system/game_state/game_state.h"

extern struct GameState *gGameState;

s32 MessageWindow_GetSoundMode(void)
{
    return (s16)gGameState->unk86C;
}
