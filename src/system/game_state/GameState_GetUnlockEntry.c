#include "global.h"
#include "system/game_state/game_state.h"

extern const u8 gUnk_08E60F4C[][4];
/* First byte of the gUnk_08E60F4C entry for unlock slot i, or 0 when the
   slot is empty (tentative). */
u8 GameState_GetUnlockEntry(s32 i)
{
    if (*(i + gGameState->table4C0) == 0)
        return 0;
    return gUnk_08E60F4C[*(i + gGameState->table4C0) - 1][0];
}
