#include "global.h"
#include "game_state.h"
s32 SceneCatalog_GetMappedState(void)
{
    s32 state = (s16)gGameState->unk12;
    if (state == 1)
        return 2;
    if (state == 2)
        return 1;
    return 0;
}
