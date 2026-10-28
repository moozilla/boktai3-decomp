#include "global.h"
#include "player.h"
u32 Script_GetKeywordValue(u32, u32);
void Player_SetField434FromScript(void)
{
    u8 *p = (*(u8 **)gPlayers);
    if (p)
        *(u16 *)(p + 0x434) = Script_GetKeywordValue(0x70, 0);
}
