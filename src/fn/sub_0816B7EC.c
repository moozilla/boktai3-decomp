#include "global.h"
#include "game_state.h"
s32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
/* Tentative: keyword 's' overrides the signed default index at +5AC. */
u32 Script_GetSelectedStateSlot(void)
{
    u32 *slot;
    if (!Script_SeekToKeyword('s')) {
        struct GameState *state = gGameState;
        slot = (u32 *)((s16)state->savedX * 4 + (u32)state);
        slot += 0x170;
    } else {
        s32 index = Script_GetValue();
        struct GameState *state = gGameState;
        slot = (u32 *)(index * 4 + (u32)state);
        slot += 0x170;
    }
    return *slot;
}
