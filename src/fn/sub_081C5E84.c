// COMPILER: old_agbcc
#include "global.h"
/* ChallengeMenu_ResetStoredRanks: Resets eight bytes of packed option values to the unset nibble value 15 */
#define ChallengeMenu_ResetStoredRanks sub_081C5E84

extern u8 *gGameState;

void sub_081C5E84(void)
{
    *(s32 *)(gGameState + 0x658) = -1;
    *(s32 *)(gGameState + 0x65c) = -1;
}
