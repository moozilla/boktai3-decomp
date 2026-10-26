#include "global.h"
#include "game_state.h"

u32 GameState_AreAllEventFlagsSet(void)
{
    u8 *base = gGameState;
    if (*(s32 *)(base + 0x6F0) == -1) {
        if (*(s32 *)(base + 0x6F4) == -1) {
            if ((*(u32 *)(base + 0x6F8) & 0x7FFFF) == 0x7FFFF) {
                return 1;
            }
        }
    }
    return 0;
}
