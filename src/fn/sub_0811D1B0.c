#include "global.h"
extern u8 *gGameState;
u8 *GameState_GetBikeName(void)
{
    return gGameState + 0x790;
}
