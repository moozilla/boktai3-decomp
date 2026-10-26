#include "global.h"

extern u8 *gGameState;
extern const u8 gUnk_08E60F4C[][4];
/* First byte of the gUnk_08E60F4C entry for unlock slot i, or 0 when the
   slot is empty (tentative). */
u8 GameState_GetUnlockEntry(s32 i)
{
    if (*(s16 *)(i * 2 + gGameState + 0x4C0) == 0)
        return 0;
    return gUnk_08E60F4C[*(s16 *)(i * 2 + gGameState + 0x4C0) - 1][0];
}
