#define Script_GetSelectedStateSlot sub_0816B7EC
#define gGameState gUnk_02000710
#include "global.h"
#include "game_state.h"
s32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
/* Tentative: keyword 's' overrides the signed default index at +5AC. */
u32 sub_0816B7EC(void)
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
