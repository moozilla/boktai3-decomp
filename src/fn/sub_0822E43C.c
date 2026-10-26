#include "global.h"
#include "game_state.h"

s16 sub_0822E43C(void)
{
    u8 *g = gGameState;
    return *(s16 *)(g + *(s16 *)(g + 0x52) * 2 + 0x88);
}
